// meego-contacts: the Nokia N9's contacts written anew in QML with the
// MeeGo Harmattan components (com.nokia.meego 2.0), on vCard files.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include "contactstore.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-contacts"));
    app.setOrganizationName(QStringLiteral("meego"));

    ContactStore contacts;

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("contacts"), &contacts);
    view.setTitle(QStringLiteral("Contacts"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
