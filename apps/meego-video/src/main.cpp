// meego-video: the Nokia N9's videos written anew in QML with the MeeGo
// Harmattan components (com.nokia.meego 2.0), on GStreamer: the videos
// with their pictures, a video full screen.
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QUrl>

#include <gst/gst.h>

#include "videoitem.h"
#include "videolibrary.h"

int main(int argc, char **argv)
{
    gst_init(&argc, &argv);
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-video"));
    app.setOrganizationName(QStringLiteral("meego"));

    qmlRegisterType<VideoItem>("MeeGo.Video", 1, 0, "Video");
    VideoLibrary library;

    // a video to play at once: meego-video /path/to.mp4
    QString open;
    for (const QString &a : app.arguments().mid(1))
        if (!a.startsWith(QLatin1String("--")) && QFileInfo::exists(a))
            open = QUrl::fromLocalFile(QFileInfo(a).canonicalFilePath()).toString();

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("library"), &library);
    view.rootContext()->setContextProperty(QStringLiteral("startVideo"), open);
    view.setTitle(QStringLiteral("Videos"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
