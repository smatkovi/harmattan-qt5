// Places by name and the address of a point: OpenStreetMap's Nominatim
// online, or OSM Scout Server's geocoder on the device (offline) where it
// answers; and the network for the map's tiles (a User-Agent as the tile
// servers ask, a cache on disk).
#ifndef PLACES_H
#define PLACES_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QSettings>
#include <QQmlNetworkAccessManagerFactory>
#include <QVariantList>
#include <functional>

class Places : public QObject
{
    Q_OBJECT
    // OSM Scout Server answers on this device
    Q_PROPERTY(bool offlineAvailable READ offlineAvailable NOTIFY offlineAvailableChanged)
    Q_PROPERTY(bool offline READ offline WRITE setOffline NOTIFY offlineChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit Places(QObject *parent = nullptr);

    bool offlineAvailable() const { return m_offlineAvailable; }
    bool offline() const { return m_offline && m_offlineAvailable; }
    void setOffline(bool on);
    bool busy() const { return m_busy > 0; }
    static QByteArray userAgent();
    static QString scoutBase();

    Q_INVOKABLE void search(const QString &text, double lat, double lon);
    Q_INVOKABLE void reverse(double lat, double lon);

signals:
    void offlineAvailableChanged();
    void offlineChanged();
    void busyChanged();
    // each {title, detail, lat, lon}; an error's text instead
    void results(const QVariantList &places, const QString &error);
    void address(double lat, double lon, const QString &title, const QString &detail);

private:
    QNetworkReply *get(const QUrl &url);
    // after the question to OSM Scout Server is answered
    void whenProbed(std::function<void()> f);
    void done();

    QNetworkAccessManager m_net;
    bool m_offlineAvailable = false, m_offline = true, m_probing = true;
    QList<std::function<void()>> m_waiting;
    int m_busy = 0;
    int m_searchSerial = 0;
};

// what is kept for the next start (~/.config/meego/meego-maps.conf)
class State : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &fallback) const;
    //! Written when it changed only, the changes of one go at once
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);

private:
    QSettings m_settings;
};

class TileNetworkFactory : public QQmlNetworkAccessManagerFactory
{
public:
    QNetworkAccessManager *create(QObject *parent) override;
};

#endif
