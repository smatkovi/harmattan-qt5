// meego-calendar: the Nokia N9's calendar written anew in QML with the
// MeeGo Harmattan components (com.nokia.meego 2.0): the month with the
// selected day's events, the event and its editor.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include "eventstore.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-calendar"));
    app.setOrganizationName(QStringLiteral("meego"));

    EventStore store;

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("store"), &store);
    view.setTitle(QStringLiteral("Calendar"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
