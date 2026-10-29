#include "places.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>
#include <QUrlQuery>

QByteArray Places::userAgent()
{
    return QByteArrayLiteral("meego-maps/0.1 (+https://github.com/smatkovi/harmattan-qt5)");
}

QString Places::scoutBase()
{
    return qEnvironmentVariableIsSet("MEEGO_MAPS_OSMSCOUT") ? qEnvironmentVariable("MEEGO_MAPS_OSMSCOUT")
                                                            : QStringLiteral("http://127.0.0.1:8553");
}

static QString nominatim()
{
    return qEnvironmentVariableIsSet("MEEGO_MAPS_NOMINATIM") ? qEnvironmentVariable("MEEGO_MAPS_NOMINATIM")
                                                             : QStringLiteral("https://nominatim.openstreetmap.org");
}

Places::Places(QObject *parent) : QObject(parent)
{
    // OSM Scout Server on this device? One short question at the start.
    QNetworkRequest req(QUrl(scoutBase() + QStringLiteral("/v1/search?limit=1&search=a")));
    req.setTransferTimeout(3000);
    QNetworkReply *r = m_net.get(req);
    connect(r, &QNetworkReply::finished, this, [this, r] {
        r->deleteLater();
        const bool ok = r->error() == QNetworkReply::NoError
                        && r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200;
        if (ok != m_offlineAvailable) {
            m_offlineAvailable = ok;
            emit offlineAvailableChanged();
            emit offlineChanged();
        }
        m_probing = false;
        const auto waiting = m_waiting;
        m_waiting.clear();
        for (const auto &f : waiting)
            f();
    });
}

void Places::whenProbed(std::function<void()> f)
{
    if (m_probing)
        m_waiting << f;
    else
        f();
}

void Places::setOffline(bool on)
{
    if (on == m_offline)
        return;
    m_offline = on;
    emit offlineChanged();
}

QNetworkReply *Places::get(const QUrl &url)
{
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", userAgent());
    req.setRawHeader("Accept-Language", QLocale().bcp47Name().toUtf8());
    req.setTransferTimeout(20000);
    if (m_busy++ == 0)
        emit busyChanged();
    return m_net.get(req);
}

void Places::done()
{
    if (--m_busy == 0)
        emit busyChanged();
}

static QString errorText(QNetworkReply *r)
{
    switch (r->error()) {
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::NetworkSessionFailedError:
    case QNetworkReply::UnknownNetworkError:
    case QNetworkReply::ConnectionRefusedError:
        return Places::tr("No connection");
    case QNetworkReply::OperationCanceledError:
    case QNetworkReply::TimeoutError:
        return Places::tr("No answer from the search");
    default:
        return r->errorString();
    }
}

void Places::search(const QString &text, double lat, double lon)
{
    const QString q = text.trimmed();
    if (q.isEmpty())
        return;
    if (m_probing)
        return whenProbed([=] { search(text, lat, lon); });
    const int serial = ++m_searchSerial;
    const bool scout = offline();
    QUrl url;
    QUrlQuery query;
    if (scout) {
        url = QUrl(scoutBase() + QStringLiteral("/v1/search"));
        query.addQueryItem(QStringLiteral("limit"), QStringLiteral("25"));
        query.addQueryItem(QStringLiteral("search"), q);
        query.addQueryItem(QStringLiteral("lat"), QString::number(lat, 'f', 5));
        query.addQueryItem(QStringLiteral("lng"), QString::number(lon, 'f', 5));
    } else {
        // the places near the map's middle first (a box to prefer, not to keep to)
        url = QUrl(nominatim() + QStringLiteral("/search"));
        query.addQueryItem(QStringLiteral("format"), QStringLiteral("jsonv2"));
        query.addQueryItem(QStringLiteral("limit"), QStringLiteral("25"));
        query.addQueryItem(QStringLiteral("q"), q);
        query.addQueryItem(QStringLiteral("viewbox"), QStringLiteral("%1,%2,%3,%4")
                           .arg(lon - 0.5, 0, 'f', 4).arg(lat + 0.5, 0, 'f', 4).arg(lon + 0.5, 0, 'f', 4).arg(lat - 0.5, 0, 'f', 4));
    }
    url.setQuery(query);
    QNetworkReply *r = get(url);
    connect(r, &QNetworkReply::finished, this, [this, r, serial, scout] {
        r->deleteLater();
        done();
        if (serial != m_searchSerial)
            return;     // an older search
        if (r->error() != QNetworkReply::NoError) {
            emit results({}, errorText(r));
            return;
        }
        QVariantList list;
        const QJsonArray array = QJsonDocument::fromJson(r->readAll()).array();
        for (const QJsonValue &v : array) {
            const QJsonObject o = v.toObject();
            QString title, detail;
            double plat, plon;
            if (scout) {
                title = o.value(QStringLiteral("title")).toString();
                detail = o.value(QStringLiteral("admin_region")).toString();
                plat = o.value(QStringLiteral("lat")).toDouble();
                plon = o.value(QStringLiteral("lng")).toDouble();
            } else {
                // "Name, street, town, …": the name, then the rest
                const QString name = o.value(QStringLiteral("display_name")).toString();
                title = o.value(QStringLiteral("name")).toString();
                const int comma = name.indexOf(QLatin1String(", "));
                if (title.isEmpty())
                    title = comma > 0 ? name.left(comma) : name;
                detail = name.startsWith(title + QLatin1String(", ")) ? name.mid(title.size() + 2) : name;
                plat = o.value(QStringLiteral("lat")).toString().toDouble();
                plon = o.value(QStringLiteral("lon")).toString().toDouble();
            }
            list << QVariantMap{{QStringLiteral("title"), title}, {QStringLiteral("detail"), detail},
                                {QStringLiteral("lat"), plat}, {QStringLiteral("lon"), plon}};
        }
        emit results(list, list.isEmpty() ? tr("Nothing found") : QString());
    });
}

void Places::reverse(double lat, double lon)
{
    if (m_probing)
        return whenProbed([=] { reverse(lat, lon); });
    const QString where = QStringLiteral("%1, %2").arg(lat, 0, 'f', 5).arg(lon, 0, 'f', 5);
    if (offline()) {
        // OSM Scout Server has no reverse lookup: the coordinates
        emit address(lat, lon, where, QString());
        return;
    }
    QUrl url(nominatim() + QStringLiteral("/reverse"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("format"), QStringLiteral("jsonv2"));
    query.addQueryItem(QStringLiteral("zoom"), QStringLiteral("18"));
    query.addQueryItem(QStringLiteral("lat"), QString::number(lat, 'f', 6));
    query.addQueryItem(QStringLiteral("lon"), QString::number(lon, 'f', 6));
    url.setQuery(query);
    QNetworkReply *r = get(url);
    connect(r, &QNetworkReply::finished, this, [this, r, lat, lon, where] {
        r->deleteLater();
        done();
        const QJsonObject o = QJsonDocument::fromJson(r->readAll()).object();
        const QString name = o.value(QStringLiteral("display_name")).toString();
        if (r->error() != QNetworkReply::NoError || name.isEmpty()) {
            emit address(lat, lon, where, QString());
            return;
        }
        const QJsonObject a = o.value(QStringLiteral("address")).toObject();
        QString title = o.value(QStringLiteral("name")).toString();
        if (title.isEmpty() && a.contains(QStringLiteral("road")))
            title = (a.value(QStringLiteral("road")).toString() + QLatin1Char(' ')
                     + a.value(QStringLiteral("house_number")).toString()).trimmed();
        if (title.isEmpty())
            title = name.section(QLatin1String(", "), 0, 0);
        QStringList rest = name.split(QLatin1String(", "));
        // the title's own parts not twice
        while (!rest.isEmpty() && title.contains(rest.first()))
            rest.removeFirst();
        emit address(lat, lon, title, rest.join(QLatin1String(", ")));
    });
}

QVariant State::value(const QString &key, const QVariant &fallback) const
{
    return QSettings().value(key, fallback);
}

void State::setValue(const QString &key, const QVariant &value)
{
    QSettings().setValue(key, value);
}

// the tiles: as the tile servers ask, who is asking; kept on disk, from
// there first
class TileNetwork : public QNetworkAccessManager
{
public:
    using QNetworkAccessManager::QNetworkAccessManager;

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &original, QIODevice *data) override
    {
        QNetworkRequest req(original);
        req.setRawHeader("User-Agent", Places::userAgent());
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        return QNetworkAccessManager::createRequest(op, req, data);
    }
};

QNetworkAccessManager *TileNetworkFactory::create(QObject *parent)
{
    auto *net = new TileNetwork(parent);
    auto *cache = new QNetworkDiskCache(net);
    cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/tiles"));
    cache->setMaximumCacheSize(200 * 1024 * 1024);
    net->setCache(cache);
    return net;
}
