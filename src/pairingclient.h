#pragma once

#include <QObject>
#include <QStringList>

class PairingClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString pin MEMBER m_pin NOTIFY pinChanged)
    Q_PROPERTY(QString status MEMBER m_status NOTIFY statusChanged)
    Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
    Q_PROPERTY(QStringList hosts MEMBER m_hosts NOTIFY hostsChanged)
    Q_PROPERTY(QStringList apps MEMBER m_apps NOTIFY appsChanged)
    Q_PROPERTY(QString appsStatus MEMBER m_appsStatus NOTIFY appsStatusChanged)
    Q_PROPERTY(bool appsBusy MEMBER m_appsBusy NOTIFY appsBusyChanged)

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
    QString m_pin;
    QString m_status;
    bool m_busy = false;
    QStringList m_hosts;
    QStringList m_apps;
    QString m_appsStatus;
    bool m_appsBusy = false;
    int m_appsGeneration = 0;
};
