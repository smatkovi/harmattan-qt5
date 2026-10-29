// meego-messages: the Nokia N9's conversations written anew in QML with
// the MeeGo Harmattan components (com.nokia.meego 2.0), on ModemManager's
// text messages.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include "sms.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-messages"));
    app.setOrganizationName(QStringLiteral("meego"));

    Sms sms;

    // a number to write to: "meego-messages 0123" or "sms:0123"
    QString number;
    for (const QString &a : app.arguments().mid(1))
        if (!a.startsWith(QLatin1String("--")))
            number = a.startsWith(QLatin1String("sms:")) ? a.mid(4) : a;

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("sms"), &sms);
    view.rootContext()->setContextProperty(QStringLiteral("startNumber"), number);
    view.setTitle(QStringLiteral("Messages"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
