// meego-mail: the Nokia N9's mail written anew in QML with the MeeGo
// Harmattan components (com.nokia.meego 2.0), on IMAP and SMTP of its own.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include "mailstore.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-mail"));
    app.setOrganizationName(QStringLiteral("meego"));

    MailStore mail;

    // an address to write to: "meego-mail mailto:x@y"
    QString to;
    for (const QString &a : app.arguments().mid(1))
        if (!a.startsWith(QLatin1String("--")))
            to = a.startsWith(QLatin1String("mailto:")) ? a.mid(7) : a;

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("mail"), &mail);
    view.rootContext()->setContextProperty(QStringLiteral("startTo"), to);
    view.setTitle(QStringLiteral("Mail"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
