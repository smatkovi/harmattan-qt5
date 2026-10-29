// meego-gallery: the Nokia N9's gallery written anew in QML with the MeeGo
// Harmattan components (com.nokia.meego 2.0): the pictures as a grid, the
// albums, a picture full screen.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QUrl>
#include <QFileInfo>

#include "pictures.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-gallery"));
    app.setOrganizationName(QStringLiteral("meego"));

    Pictures pictures;

    // a picture to show at once: meego-gallery /path/to.jpg
    QString open;
    for (const QString &a : app.arguments().mid(1))
        if (!a.startsWith(QLatin1String("--")) && QFileInfo::exists(a))
            open = QUrl::fromLocalFile(QFileInfo(a).canonicalFilePath()).toString();

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("pictures"), &pictures);
    view.rootContext()->setContextProperty(QStringLiteral("startPicture"), open);
    view.setTitle(QStringLiteral("Gallery"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
