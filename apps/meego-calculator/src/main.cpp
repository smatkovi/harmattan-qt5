// meego-calculator: the Nokia N9's calculator written anew in QML with the
// MeeGo Harmattan components (com.nokia.meego 2.0): the calculation line
// above, the keys below, dark.
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQuickView>
#include <QUrl>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-calculator"));
    app.setOrganizationName(QStringLiteral("meego"));

    QQuickView view;
    view.setTitle(QStringLiteral("Calculator"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
