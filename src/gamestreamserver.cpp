#include "gamestreamserver.h"

extern "C" {
#include "errors.h"
}

#include <QCoreApplication>
#include <QMutex>

#include <cstdlib>

GameStreamServer::GameStreamServer(const QByteArray &host, const QByteArray &keyDirectory)
    : _locker(&mutex())
    , _host(host)
    , _keyDirectory(keyDirectory)
{
}

GameStreamServer::~GameStreamServer()
{
    release();
}

QMutex &GameStreamServer::mutex()
{
    static QMutex mutex;
    return mutex;
}

bool GameStreamServer::open()
{
    const int code = gs_init(&_server, _host.data(), 0, _keyDirectory.constData(), 0, true);
    if (code == GS_OK)
        return true;
    _error = errorString(code);
    release();
    return false;
}

bool GameStreamServer::isPaired() const
{
    return _server.paired;
}

bool GameStreamServer::pair(const QByteArray &pin)
{
    QByteArray buffer = pin;
    const int code = gs_pair(&_server, buffer.data());
    if (code == GS_OK)
        return true;
    _error = errorString(code);
    return false;
}

bool GameStreamServer::appList(QStringList *apps)
{
    PAPP_LIST list = nullptr;
    const int code = gs_applist(&_server, &list);
    if (code != GS_OK) {
        _error = errorString(code);
        freeAppList(list);
        return false;
    }

    for (PAPP_LIST app = list; app != nullptr; app = app->next) {
        QString name = app->name != nullptr ? QString::fromUtf8(app->name) : QString();
        if (name.isEmpty())
            name = QString::number(app->id);
        apps->prepend(name);
    }
    freeAppList(list);
    return true;
}

QString GameStreamServer::error() const
{
    return _error;
}

QString GameStreamServer::errorString(int code) const
{
    if (gs_error != nullptr && gs_error[0] != '\0')
        return QString::fromUtf8(gs_error);
    if (code == GS_IO_ERROR)
        return QCoreApplication::translate("GameStreamServer", "Cannot reach the computer");
    return QCoreApplication::translate("GameStreamServer", "Request failed");
}

void GameStreamServer::freeAppList(PAPP_LIST list)
{
    while (list != nullptr) {
        PAPP_LIST next = list->next;
        std::free(list->name);
        std::free(list);
        list = next;
    }
}

void GameStreamServer::release()
{
    std::free(_server.gpuType);
    std::free(_server.gsVersion);
    std::free(const_cast<char *>(_server.serverInfo.serverInfoAppVersion));
    std::free(const_cast<char *>(_server.serverInfo.serverInfoGfeVersion));
    std::free(const_cast<char *>(_server.serverInfo.rtspSessionUrl));
    PDISPLAY_MODE mode = _server.modes;
    while (mode != nullptr) {
        PDISPLAY_MODE next = mode->next;
        std::free(mode);
        mode = next;
    }
    _server = {};
}
