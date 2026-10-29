// meego-music: the Nokia N9's music player written anew in QML with the
// MeeGo Harmattan components (com.nokia.meego 2.0), on GStreamer: the
// covers and the library's views, the lists, now playing.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include <gst/gst.h>

#include "library.h"
#include "player.h"

int main(int argc, char **argv)
{
    gst_init(&argc, &argv);
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-music"));
    app.setOrganizationName(QStringLiteral("meego"));

    Library library;
    Player player(&library);

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("library"), &library);
    view.rootContext()->setContextProperty(QStringLiteral("player"), &player);
    view.setTitle(QStringLiteral("Music"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
