#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QScopedPointer>
#include <QSharedPointer>
#include <sailfishapp.h>

#include "pairingclient.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setOrganizationName(QStringLiteral("moonlight-sfos"));
    app->setApplicationName(QStringLiteral("moonlight-sfos"));

    PairingClient pairing;
    QSharedPointer<QQuickView> view(SailfishApp::createView());
    view->rootContext()->setContextProperty(QStringLiteral("pairing"), &pairing);

    view->setSource(SailfishApp::pathTo("qml/moonlight-sfos.qml"));
    view->show();

    return app->exec();
}
