// The music library: the audio files below the music folder, their tags
// and covers (GStreamer's discoverer), cached between starts, with the
// play counts the smart playlists use.
#ifndef LIBRARY_H
#define LIBRARY_H

#include <QFutureWatcher>
#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class Library : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(int count READ count NOTIFY changed)
    Q_PROPERTY(QString folder READ folder CONSTANT)

public:
    explicit Library(QObject *parent = nullptr);

    bool scanning() const { return m_watcher.isRunning(); }
    int count() const { return m_songs.size(); }
    QString folder() const { return m_folder; }

    // each song: url, title, artist, album, genre, track, duration (ms),
    // cover (a file URL or ""), added (seconds)
    Q_INVOKABLE QVariantList songs() const;
    Q_INVOKABLE QVariantList artists() const;       // name, albums, songs, cover
    Q_INVOKABLE QVariantList albums(const QString &artist = QString()) const;  // name, artist, cover, songs
    Q_INVOKABLE QVariantList genres() const;        // name, songs
    Q_INVOKABLE QVariantList albumSongs(const QString &album, const QString &artist) const;
    Q_INVOKABLE QVariantList artistSongs(const QString &artist) const;
    Q_INVOKABLE QVariantList genreSongs(const QString &genre) const;
    // the smart playlists: "recent" (added), "played" (lately), "most"
    Q_INVOKABLE QVariantList playlist(const QString &which) const;
    // covers for the main view's mosaic, the lately played first
    Q_INVOKABLE QStringList mosaic(int n) const;

    Q_INVOKABLE void rescan();
    Q_INVOKABLE void played(const QString &url);

signals:
    void changed();
    void scanningChanged();

private:
    void load();
    void saveCache() const;
    void saveStats() const;
    QVariantList sorted(QVariantList list, const char *key) const;

    QString m_folder, m_cacheDir;
    QVariantList m_songs;
    QVariantMap m_plays, m_lastPlayed;   // url -> count, url -> seconds
    QFutureWatcher<QVariantList> m_watcher;
};

#endif
