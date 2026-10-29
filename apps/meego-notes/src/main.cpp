// meego-notes: the Nokia N9's notes written anew in QML with the MeeGo
// Harmattan components (com.nokia.meego 2.0): the notes as a list, each
// written in a sheet.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QUrl>

#include "notestore.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-notes"));
    app.setOrganizationName(QStringLiteral("meego"));

    NoteStore notes;

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("notes"), &notes);
    // meego-notes --new: a new note at once
    view.rootContext()->setContextProperty(QStringLiteral("startNew"), app.arguments().contains(QStringLiteral("--new")));
    view.setTitle(QStringLiteral("Notes"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
