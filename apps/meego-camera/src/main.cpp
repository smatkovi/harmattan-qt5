// meego-camera: the Nokia N9's camera written anew in QML with the MeeGo
// Harmattan components (com.nokia.meego 2.0), on GStreamer and libcamera:
// the viewfinder full screen, photos and videos.
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQuickView>
#include <QUrl>

#include <gst/gst.h>

#include "viewfinder.h"

int main(int argc, char **argv)
{
    gst_init(&argc, &argv);
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-camera"));
    app.setOrganizationName(QStringLiteral("meego"));

    qmlRegisterType<Viewfinder>("MeeGo.Camera", 1, 0, "Viewfinder");

    QQuickView view;
    view.setTitle(QStringLiteral("Camera"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
