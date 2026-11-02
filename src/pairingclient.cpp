#include "pairingclient.h"
#include "gamestreamserver.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QThread>

namespace {

class PairJob : public QThread
{
public:
    QByteArray host;
    QByteArray pin;
    QByteArray keyDirectory;
    QString error;
    bool ok = false;

    void run() override
    {
        GameStreamServer server(host, keyDirectory);
        if (!server.open()) {
            error = server.error();
            return;
        }
        ok = server.isPaired() || server.pair(pin);
        if (!ok)
            error = server.error();
    }
};

class AppListJob : public QThread
{
public:
    QByteArray host;
    QByteArray keyDirectory;
    QStringList apps;
    QString error;
    bool ok = false;

    void run() override
    {
        GameStreamServer server(host, keyDirectory);
        if (!server.open()) {
            error = server.error();
            return;
        }
        if (!server.isPaired()) {
            error = PairingClient::tr("This computer is not paired");
            return;
        }
        ok = server.appList(&apps);
        if (!ok)
            error = server.error();
    }
};

} // namespace

PairingClient::PairingClient(QObject *parent)
    : QObject(parent)
{
    loadHosts();
}

QString PairingClient::appDataPath()
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);
    return directory;
}

QString PairingClient::hostsFilePath()
{
    return appDataPath() + QStringLiteral("/hosts");
}

QString PairingClient::makePin()
{
    return QString::asprintf("%04lld", QDateTime::currentMSecsSinceEpoch() % 10000);
}

void PairingClient::loadHosts()
{
    QFile file(hostsFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return;

    while (!file.atEnd()) {
        const QString address = QString::fromUtf8(file.readLine()).trimmed();
        if (!address.isEmpty() && !_hosts.contains(address))
            _hosts.append(address);
    }
}

void PairingClient::saveHosts() const
{
    QFile file(hostsFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    QTextStream stream(&file);
    for (const QString &host : _hosts)
        stream << host << '\n';
}

void PairingClient::wipe()
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!directory.isEmpty())
        QDir(directory).removeRecursively();
    QCoreApplication::quit();
}

void PairingClient::pair(const QString &host)
{
    const QString address = host.trimmed();
    if (_busy || address.isEmpty())
        return;

    _pin = makePin();
    _status = tr("Enter this PIN in Sunshine");
    _busy = true;
    emit pinChanged();
    emit statusChanged();
    emit busyChanged();

    auto *job = new PairJob;
    job->host = address.toUtf8();
    job->pin = _pin.toLatin1();
    job->keyDirectory = appDataPath().toUtf8();
    connect(job, &QThread::finished, this, [this, job] {
        _busy = false;
        emit busyChanged();
        if (job->ok && !_hosts.contains(QString::fromUtf8(job->host))) {
            _hosts.append(QString::fromUtf8(job->host));
            saveHosts();
            emit hostsChanged();
        }
        _status = job->ok ? tr("Paired with %1").arg(QString::fromUtf8(job->host)) : job->error;
        emit statusChanged();
        job->deleteLater();
    });
    job->start();
}

void PairingClient::loadApps(const QString &host)
{
    const QString address = host.trimmed();
    if (address.isEmpty())
        return;

    const int generation = ++_appsGeneration;
    _apps.clear();
    _appsStatus.clear();
    _appsBusy = true;
    emit appsChanged();
    emit appsStatusChanged();
    emit appsBusyChanged();

    auto *job = new AppListJob;
    job->host = address.toUtf8();
    job->keyDirectory = appDataPath().toUtf8();
    connect(job, &QThread::finished, this, [this, job, generation] {
        if (generation != _appsGeneration) {
            job->deleteLater();
            return;
        }
        _appsBusy = false;
        if (job->ok)
            _apps = job->apps;
        else
            _appsStatus = job->error;
        emit appsBusyChanged();
        emit appsChanged();
        emit appsStatusChanged();
        job->deleteLater();
    });
    job->start();
}
