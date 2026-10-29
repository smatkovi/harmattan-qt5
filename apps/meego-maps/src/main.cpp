// meego-maps: the Nokia N9's maps written anew in QML with the MeeGo
// Harmattan components (com.nokia.meego 2.0): OpenStreetMap's tiles (or
// OSM Scout Server's on the device), one's position, places by name.
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QUrl>
#include <QUrlQuery>

#include "places.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("meego-maps"));
    app.setOrganizationName(QStringLiteral("meego"));

    Places places;
    State state;

    // a place to show at once: meego-maps geo:47.56,13.65
    QVariantMap open;
    for (const QString &a : app.arguments().mid(1)) {
        if (!a.startsWith(QLatin1String("geo:")))
            continue;
        const QStringList ll = a.mid(4).section(QLatin1Char('?'), 0, 0).section(QLatin1Char(';'), 0, 0).split(QLatin1Char(','));
        bool ok1 = false, ok2 = false;
        if (ll.size() >= 2) {
            const double lat = ll.at(0).toDouble(&ok1), lon = ll.at(1).toDouble(&ok2);
            if (ok1 && ok2)
                open = QVariantMap{{QStringLiteral("lat"), lat}, {QStringLiteral("lon"), lon},
                                   {QStringLiteral("label"), QUrlQuery(a.section(QLatin1Char('?'), 1)).queryItemValue(QStringLiteral("q"), QUrl::FullyDecoded)}};
        }
    }

    QQuickView view;
    view.engine()->setNetworkAccessManagerFactory(new TileNetworkFactory);
    view.rootContext()->setContextProperty(QStringLiteral("places"), &places);
    view.rootContext()->setContextProperty(QStringLiteral("stateStore"), &state);
    view.rootContext()->setContextProperty(QStringLiteral("startPlace"), open);
    view.rootContext()->setContextProperty(QStringLiteral("osmScout"), Places::scoutBase());
    view.rootContext()->setContextProperty(QStringLiteral("tileOverride"), qEnvironmentVariable("MEEGO_MAPS_TILES"));
    view.setTitle(QStringLiteral("Maps"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (app.arguments().contains(QStringLiteral("--fullscreen")))
        view.showFullScreen();
    else
        view.show();
    return app.exec();
}
