#pragma once

#include <QObject>
#include <QStringList>

class PairingClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString pin MEMBER _pin NOTIFY pinChanged)
    Q_PROPERTY(QString status MEMBER _status NOTIFY statusChanged)
    Q_PROPERTY(bool busy MEMBER _busy NOTIFY busyChanged)
    Q_PROPERTY(QStringList hosts MEMBER _hosts NOTIFY hostsChanged)
    Q_PROPERTY(QStringList apps MEMBER _apps NOTIFY appsChanged)
    Q_PROPERTY(QString appsStatus MEMBER _appsStatus NOTIFY appsStatusChanged)
    Q_PROPERTY(bool appsBusy MEMBER _appsBusy NOTIFY appsBusyChanged)

public:
    explicit PairingClient(QObject *parent = nullptr);

    Q_INVOKABLE void pair(const QString &host);
    Q_INVOKABLE void loadApps(const QString &host);
    Q_INVOKABLE void wipe();

signals:
    void pinChanged();
    void statusChanged();
    void busyChanged();
    void hostsChanged();
    void appsChanged();
    void appsStatusChanged();
    void appsBusyChanged();

private:
    static QString appDataPath();
    static QString hostsFilePath();
    static QString makePin();
    void loadHosts();
    void saveHosts() const;

    QString _pin;
    QString _status;
    bool _busy = false;
    QStringList _hosts;
    QStringList _apps;
    QString _appsStatus;
    bool _appsBusy = false;
    int _appsGeneration = 0;
};
