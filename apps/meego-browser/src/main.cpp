// meego-browser: the Nokia N9's web browser written anew in QML with the
// MeeGo Harmattan components (com.nokia.meego 2.0), on Qt WebEngine or
// Qt WebKit (whichever QML module is installed): the address field above,
// the page, the start view with the bookmarks and the history.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QUrl>

#include "browserstore.h"

int main(int argc, char **argv)
{
    // what QtWebEngine::initialize() does, without linking it: its view
    // shares the GL context
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-browser"));
    app.setOrganizationName(QStringLiteral("meego"));

    BrowserStore store;

    // an address to open at once: meego-browser https://example.org
    QString open;
    for (const QString &a : app.arguments().mid(1))
        if (!a.startsWith(QLatin1String("--")))
            open = store.fixUrl(a);

    // the web views to try, in order (a test: MEEGO_BROWSER_VIEW=file:///…/FakeView.qml)
    QStringList views{QStringLiteral("qrc:/qml/WebEngineBackend.qml"), QStringLiteral("qrc:/qml/WebKitBackend.qml")};
    const QString engine = qEnvironmentVariable("MEEGO_BROWSER_ENGINE");
    if (engine == QLatin1String("webkit"))
        views.removeFirst();
    else if (engine == QLatin1String("webengine"))
        views.removeLast();
    if (qEnvironmentVariableIsSet("MEEGO_BROWSER_VIEW"))
        views = QStringList{qEnvironmentVariable("MEEGO_BROWSER_VIEW")};

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("store"), &store);
    view.rootContext()->setContextProperty(QStringLiteral("startUrl"), open);
    view.rootContext()->setContextProperty(QStringLiteral("webViews"), views);
    view.setTitle(QStringLiteral("Web"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
