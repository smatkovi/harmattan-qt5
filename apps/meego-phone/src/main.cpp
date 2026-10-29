// meego-phone: the Nokia N9's phone written anew in QML with the MeeGo
// Harmattan components (com.nokia.meego 2.0), on ModemManager: the
// dialler, the call history and the call screen.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include "audioroute.h"
#include "callhistory.h"
#include "modem.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-phone"));
    app.setOrganizationName(QStringLiteral("meego"));

    Modem modem;
    CallHistory history;
    AudioRoute audio;
    QObject::connect(&modem, &Modem::callEnded, &history, &CallHistory::add);
    QObject::connect(&modem, &Modem::callEnded, &audio, &AudioRoute::reset);

    // a number to dial: "meego-phone 0123" or "meego-phone tel:0123"
    QString number;
    for (const QString &a : app.arguments().mid(1))
        if (!a.startsWith(QLatin1String("--")))
            number = a.startsWith(QLatin1String("tel:")) ? a.mid(4) : a;

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("modem"), &modem);
    view.rootContext()->setContextProperty(QStringLiteral("history"), &history);
    view.rootContext()->setContextProperty(QStringLiteral("audio"), &audio);
    view.rootContext()->setContextProperty(QStringLiteral("startNumber"), number);
    view.setTitle(QStringLiteral("Phone"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
