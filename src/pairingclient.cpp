#include "pairingclient.h"

extern "C" {
#include "client.h"
#include "errors.h"
}

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QTextStream>
#include <QThread>

#include <cstdlib>

namespace {

QMutex &gamestreamMutex()
{
    static QMutex mutex;
    return mutex;
}

QString appDataPath()
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);
    return directory;
}

QStringList loadHostList()
{
    QStringList hosts;
    QFile file(appDataPath() + QStringLiteral("/hosts"));
    if (!file.open(QIODevice::ReadOnly))
        return hosts;

    while (!file.atEnd()) {
        const QString address = QString::fromUtf8(file.readLine()).trimmed();
        if (!address.isEmpty() && !hosts.contains(address))
            hosts.append(address);
    }
    return hosts;
}

void saveHostList(const QStringList &hosts)
{
    QFile file(appDataPath() + QStringLiteral("/hosts"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    QTextStream stream(&file);
    for (const QString &host : hosts)
        stream << host << '\n';
}

QString makePin()
{
    return QString::asprintf("%04lld", QDateTime::currentMSecsSinceEpoch() % 10000);
}

QString gamestreamError(int code)
{
    if (gs_error != nullptr && gs_error[0] != '\0')
        return QString::fromUtf8(gs_error);
    if (code == GS_IO_ERROR)
        return PairingClient::tr("Cannot reach the computer");
    return PairingClient::tr("Request failed");
}

void freeAppList(PAPP_LIST list)
{
    while (list != nullptr) {
        PAPP_LIST next = list->next;
        free(list->name);
        free(list);
        list = next;
    }
}

void freeServer(SERVER_DATA *server)
{
    free(server->gpuType);
    free(server->gsVersion);
    free(const_cast<char *>(server->serverInfo.serverInfoAppVersion));
    free(const_cast<char *>(server->serverInfo.serverInfoGfeVersion));
    free(const_cast<char *>(server->serverInfo.rtspSessionUrl));
    PDISPLAY_MODE mode = server->modes;
    while (mode != nullptr) {
        PDISPLAY_MODE next = mode->next;
        free(mode);
        mode = next;
    }
    server->modes = nullptr;
}

bool openServer(SERVER_DATA *server, QByteArray &host, const QByteArray &keyDirectory, QString *error)
{
    const int code = gs_init(server, host.data(), 0, keyDirectory.constData(), 0, true);
    if (code == GS_OK)
        return true;
    *error = gamestreamError(code);
    freeServer(server);
    return false;
}

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
        const QMutexLocker locker(&gamestreamMutex());
        SERVER_DATA server = {};
        if (!openServer(&server, host, keyDirectory, &error))
            return;
        if (server.paired || gs_pair(&server, pin.data()) == GS_OK)
            ok = true;
        else
            error = gamestreamError(GS_FAILED);
        freeServer(&server);
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
        const QMutexLocker locker(&gamestreamMutex());
        SERVER_DATA server = {};
        if (!openServer(&server, host, keyDirectory, &error))
            return;

        if (!server.paired) {
            error = PairingClient::tr("This computer is not paired");
        } else {
            PAPP_LIST list = nullptr;
            const int listed = gs_applist(&server, &list);
            if (listed == GS_OK) {
                for (PAPP_LIST app = list; app != nullptr; app = app->next) {
                    QString name = app->name != nullptr ? QString::fromUtf8(app->name) : QString();
                    if (name.isEmpty())
                        name = QString::number(app->id);
                    apps.prepend(name);
                }
                ok = true;
            } else {
                error = gamestreamError(listed);
            }
            freeAppList(list);
        }
        freeServer(&server);
    }
};

} // namespace

PairingClient::PairingClient(QObject *parent)
    : QObject(parent)
{
    m_hosts = loadHostList();
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
    if (m_busy || address.isEmpty())
        return;

    m_pin = makePin();
    m_status = tr("Enter this PIN in Sunshine");
    m_busy = true;
    emit pinChanged();
    emit statusChanged();
    emit busyChanged();

    auto *job = new PairJob;
    job->host = address.toUtf8();
    job->pin = m_pin.toLatin1();
    job->keyDirectory = appDataPath().toUtf8();
    connect(job, &QThread::finished, this, [this, job] {
        m_busy = false;
        emit busyChanged();
        if (job->ok && !m_hosts.contains(QString::fromUtf8(job->host))) {
            m_hosts.append(QString::fromUtf8(job->host));
            saveHostList(m_hosts);
            emit hostsChanged();
        }
        m_status = job->ok ? tr("Paired with %1").arg(QString::fromUtf8(job->host)) : job->error;
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

    const int generation = ++m_appsGeneration;
    m_apps.clear();
    m_appsStatus.clear();
    m_appsBusy = true;
    emit appsChanged();
    emit appsStatusChanged();
    emit appsBusyChanged();

    auto *job = new AppListJob;
    job->host = address.toUtf8();
    job->keyDirectory = appDataPath().toUtf8();
    connect(job, &QThread::finished, this, [this, job, generation] {
        if (generation != m_appsGeneration) {
            job->deleteLater();
            return;
        }
        m_appsBusy = false;
        if (job->ok)
            m_apps = job->apps;
        else
            m_appsStatus = job->error;
        emit appsBusyChanged();
        emit appsChanged();
        emit appsStatusChanged();
        job->deleteLater();
    });
    job->start();
}
