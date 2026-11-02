#pragma once

#include <QByteArray>
#include <QMutexLocker>
#include <QString>
#include <QStringList>

extern "C" {
#include "client.h"
}

// One libgamestream session with a host. libgamestream is not thread-safe
// (gs_error is global), so only one GameStreamServer can be alive at a time.
class GameStreamServer
{
    Q_DISABLE_COPY(GameStreamServer)

public:
    GameStreamServer(const QByteArray &host, const QByteArray &keyDirectory);
    ~GameStreamServer();

    bool open();
    bool isPaired() const;
    bool pair(const QByteArray &pin);
    bool appList(QStringList *apps);
    QString error() const;

private:
    static QMutex &mutex();
    static void freeAppList(PAPP_LIST list);
    QString errorString(int code) const;
    void release();

    QMutexLocker _locker;
    QByteArray _host;
    QByteArray _keyDirectory;
    SERVER_DATA _server = {};
    QString _error;
};
