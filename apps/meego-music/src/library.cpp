#include "library.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>
#include <algorithm>

#include <gst/gst.h>
#include <gst/pbutils/pbutils.h>
#include <gst/tag/tag.h>

static const QStringList audioFilters = {
    QStringLiteral("*.mp3"), QStringLiteral("*.ogg"), QStringLiteral("*.oga"),
    QStringLiteral("*.opus"), QStringLiteral("*.flac"), QStringLiteral("*.m4a"),
    QStringLiteral("*.aac"), QStringLiteral("*.wav"), QStringLiteral("*.wma"),
};

static QString tagString(const GstTagList *tags, const char *tag)
{
    gchar *s = nullptr;
    if (!tags || !gst_tag_list_get_string(tags, tag, &s))
        return QString();
    QString r = QString::fromUtf8(s).trimmed();
    g_free(s);
    return r;
}

// A cover: the image in the file's tags, else a picture beside it
static QString coverFor(const GstTagList *tags, const QFileInfo &file, const QString &coverDir,
                        const QString &key)
{
    const QString base = coverDir + QLatin1Char('/')
        + QString::fromLatin1(QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5).toHex());
    for (const char *ext : {".jpg", ".png"})
        if (QFile::exists(base + QLatin1String(ext)))
            return QUrl::fromLocalFile(base + QLatin1String(ext)).toString();

    GstSample *sample = nullptr;
    if (tags && (gst_tag_list_get_sample(tags, GST_TAG_IMAGE, &sample)
                 || gst_tag_list_get_sample(tags, GST_TAG_PREVIEW_IMAGE, &sample))) {
        GstBuffer *buf = gst_sample_get_buffer(sample);
        GstCaps *caps = gst_sample_get_caps(sample);
        const gchar *mime = caps ? gst_structure_get_name(gst_caps_get_structure(caps, 0)) : "";
        GstMapInfo map;
        QString path;
        if (buf && gst_buffer_map(buf, &map, GST_MAP_READ)) {
            path = base + (g_str_has_suffix(mime, "png") ? QStringLiteral(".png") : QStringLiteral(".jpg"));
            QFile f(path);
            if (f.open(QIODevice::WriteOnly))
                f.write(reinterpret_cast<const char *>(map.data), map.size);
            gst_buffer_unmap(buf, &map);
        }
        gst_sample_unref(sample);
        if (!path.isEmpty())
            return QUrl::fromLocalFile(path).toString();
    }
    const QDir dir = file.dir();
    for (const QString &name : dir.entryList({QStringLiteral("cover.*"), QStringLiteral("folder.*"),
                                              QStringLiteral("front.*"), QStringLiteral("Cover.*"),
                                              QStringLiteral("Folder.*")}, QDir::Files)) {
        if (name.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive)
            || name.endsWith(QLatin1String(".jpeg"), Qt::CaseInsensitive)
            || name.endsWith(QLatin1String(".png"), Qt::CaseInsensitive))
            return QUrl::fromLocalFile(dir.filePath(name)).toString();
    }
    return QString();
}

// In a worker thread: the folder's audio files, the cached ones as they
// were when unchanged, the others through the discoverer
static QVariantList scanFolder(const QString &folder, const QString &coverDir, const QVariantList &cached)
{
    QHash<QString, QVariantMap> old;
    for (const QVariant &v : cached) {
        const QVariantMap m = v.toMap();
        old.insert(m.value(QStringLiteral("path")).toString(), m);
    }
    QDir().mkpath(coverDir);
    GstDiscoverer *disc = gst_discoverer_new(5 * GST_SECOND, nullptr);
    QVariantList list;
    QDirIterator it(folder, audioFilters, QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    while (it.hasNext()) {
        const QString path = it.next();
        const QFileInfo fi(path);
        const qint64 mtime = fi.lastModified().toSecsSinceEpoch();
        const QVariantMap prev = old.value(path);
        if (!prev.isEmpty() && prev.value(QStringLiteral("mtime")).toLongLong() == mtime) {
            list << prev;
            continue;
        }
        QVariantMap m;
        m[QStringLiteral("path")] = path;
        m[QStringLiteral("url")] = QUrl::fromLocalFile(path).toString();
        m[QStringLiteral("mtime")] = mtime;
        m[QStringLiteral("added")] = prev.value(QStringLiteral("added"), QDateTime::currentSecsSinceEpoch());
        QString title, artist, album, genre;
        int track = 0;
        qint64 duration = 0;
        GstTagList *tags = nullptr;
        GstDiscovererInfo *info = disc ? gst_discoverer_discover_uri(
            disc, QUrl::fromLocalFile(path).toEncoded().constData(), nullptr) : nullptr;
        if (info) {
            // the container's and the streams' tags together: newer
            // GStreamer keeps a file's tags with its audio stream
            tags = gst_tag_list_new_empty();
            GList *streams = gst_discoverer_info_get_stream_list(info);
            for (GList *l = streams; l; l = l->next) {
                const GstTagList *t = gst_discoverer_stream_info_get_tags(GST_DISCOVERER_STREAM_INFO(l->data));
                if (t)
                    gst_tag_list_insert(tags, t, GST_TAG_MERGE_KEEP);
            }
            gst_discoverer_stream_info_list_free(streams);
            title = tagString(tags, GST_TAG_TITLE);
            artist = tagString(tags, GST_TAG_ARTIST);
            album = tagString(tags, GST_TAG_ALBUM);
            genre = tagString(tags, GST_TAG_GENRE);
            guint t = 0;
            if (tags && gst_tag_list_get_uint(tags, GST_TAG_TRACK_NUMBER, &t))
                track = int(t);
            duration = qint64(gst_discoverer_info_get_duration(info) / GST_MSECOND);
        }
        if (title.isEmpty())
            title = fi.completeBaseName();
        m[QStringLiteral("title")] = title;
        m[QStringLiteral("artist")] = artist;
        m[QStringLiteral("album")] = album;
        m[QStringLiteral("genre")] = genre;
        m[QStringLiteral("track")] = track;
        m[QStringLiteral("duration")] = duration;
        m[QStringLiteral("cover")] = coverFor(tags, fi, coverDir,
            (album.isEmpty() ? fi.absolutePath() : album) + QLatin1Char('|') + artist);
        if (tags)
            gst_tag_list_unref(tags);
        if (info)
            g_object_unref(info);
        list << m;
    }
    if (disc)
        g_object_unref(disc);
    return list;
}

Library::Library(QObject *parent) : QObject(parent)
{
    m_folder = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    if (qEnvironmentVariableIsSet("MEEGO_MUSIC_DIR"))
        m_folder = qEnvironmentVariable("MEEGO_MUSIC_DIR");
    m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(m_cacheDir);
    connect(&m_watcher, &QFutureWatcher<QVariantList>::finished, this, [this] {
        m_songs = m_watcher.result();
        saveCache();
        emit scanningChanged();
        emit changed();
    });
    load();
    rescan();
}

void Library::load()
{
    QFile f(m_cacheDir + QStringLiteral("/library.json"));
    if (f.open(QIODevice::ReadOnly))
        m_songs = QJsonDocument::fromJson(f.readAll()).array().toVariantList();
    QFile s(m_cacheDir + QStringLiteral("/stats.json"));
    if (s.open(QIODevice::ReadOnly)) {
        const QJsonObject o = QJsonDocument::fromJson(s.readAll()).object();
        m_plays = o.value(QStringLiteral("plays")).toObject().toVariantMap();
        m_lastPlayed = o.value(QStringLiteral("last")).toObject().toVariantMap();
    }
}

void Library::saveCache() const
{
    QFile f(m_cacheDir + QStringLiteral("/library.json"));
    if (f.open(QIODevice::WriteOnly))
        f.write(QJsonDocument(QJsonArray::fromVariantList(m_songs)).toJson(QJsonDocument::Compact));
}

void Library::saveStats() const
{
    QFile f(m_cacheDir + QStringLiteral("/stats.json"));
    if (f.open(QIODevice::WriteOnly))
        f.write(QJsonDocument(QJsonObject{
            {QStringLiteral("plays"), QJsonObject::fromVariantMap(m_plays)},
            {QStringLiteral("last"), QJsonObject::fromVariantMap(m_lastPlayed)}}).toJson());
}

void Library::rescan()
{
    if (m_watcher.isRunning())
        return;
    m_watcher.setFuture(QtConcurrent::run(scanFolder, m_folder, m_cacheDir + QStringLiteral("/covers"), m_songs));
    emit scanningChanged();
}

void Library::played(const QString &url)
{
    m_plays[url] = m_plays.value(url).toInt() + 1;
    m_lastPlayed[url] = QDateTime::currentSecsSinceEpoch();
    saveStats();
}

static QString name(const QVariantMap &m, const char *key, const QString &unknown)
{
    const QString v = m.value(QLatin1String(key)).toString();
    return v.isEmpty() ? unknown : v;
}

QVariantList Library::sorted(QVariantList list, const char *key) const
{
    std::sort(list.begin(), list.end(), [key](const QVariant &a, const QVariant &b) {
        return QString::localeAwareCompare(a.toMap().value(QLatin1String(key)).toString(),
                                           b.toMap().value(QLatin1String(key)).toString()) < 0;
    });
    return list;
}

QVariantList Library::songs() const
{
    return sorted(m_songs, "title");
}

QVariantList Library::artists() const
{
    QMap<QString, QVariantMap> by;
    QHash<QString, QSet<QString>> albums;
    for (const QVariant &v : m_songs) {
        const QVariantMap s = v.toMap();
        const QString a = name(s, "artist", tr("Unknown artist"));
        QVariantMap &m = by[a];
        m[QStringLiteral("name")] = a;
        m[QStringLiteral("songs")] = m.value(QStringLiteral("songs")).toInt() + 1;
        if (m.value(QStringLiteral("cover")).toString().isEmpty())
            m[QStringLiteral("cover")] = s.value(QStringLiteral("cover"));
        albums[a].insert(s.value(QStringLiteral("album")).toString());
    }
    QVariantList list;
    for (auto it = by.begin(); it != by.end(); ++it) {
        it.value()[QStringLiteral("albums")] = albums.value(it.key()).size();
        list << it.value();
    }
    return sorted(list, "name");
}

QVariantList Library::albums(const QString &artist) const
{
    QMap<QString, QVariantMap> by;
    for (const QVariant &v : m_songs) {
        const QVariantMap s = v.toMap();
        const QString ar = name(s, "artist", tr("Unknown artist"));
        if (!artist.isEmpty() && ar != artist)
            continue;
        const QString al = name(s, "album", tr("Unknown album"));
        QVariantMap &m = by[al + QLatin1Char('|') + ar];
        m[QStringLiteral("name")] = al;
        m[QStringLiteral("artist")] = ar;
        m[QStringLiteral("songs")] = m.value(QStringLiteral("songs")).toInt() + 1;
        if (m.value(QStringLiteral("cover")).toString().isEmpty())
            m[QStringLiteral("cover")] = s.value(QStringLiteral("cover"));
    }
    QVariantList list;
    for (const QVariantMap &m : by)
        list << m;
    return sorted(list, "name");
}

QVariantList Library::genres() const
{
    QMap<QString, int> by;
    for (const QVariant &v : m_songs)
        by[name(v.toMap(), "genre", tr("Unknown genre"))]++;
    QVariantList list;
    for (auto it = by.begin(); it != by.end(); ++it)
        list << QVariantMap{{QStringLiteral("name"), it.key()}, {QStringLiteral("songs"), it.value()}};
    return sorted(list, "name");
}

QVariantList Library::albumSongs(const QString &album, const QString &artist) const
{
    QVariantList list;
    for (const QVariant &v : m_songs) {
        const QVariantMap s = v.toMap();
        if (name(s, "album", tr("Unknown album")) == album && name(s, "artist", tr("Unknown artist")) == artist)
            list << s;
    }
    std::sort(list.begin(), list.end(), [](const QVariant &a, const QVariant &b) {
        const int ta = a.toMap().value(QStringLiteral("track")).toInt();
        const int tb = b.toMap().value(QStringLiteral("track")).toInt();
        if (ta != tb)
            return ta < tb;
        return a.toMap().value(QStringLiteral("title")).toString() < b.toMap().value(QStringLiteral("title")).toString();
    });
    return list;
}

QVariantList Library::artistSongs(const QString &artist) const
{
    QVariantList list;
    for (const QVariant &v : m_songs)
        if (name(v.toMap(), "artist", tr("Unknown artist")) == artist)
            list << v;
    return sorted(list, "title");
}

QVariantList Library::genreSongs(const QString &genre) const
{
    QVariantList list;
    for (const QVariant &v : m_songs)
        if (name(v.toMap(), "genre", tr("Unknown genre")) == genre)
            list << v;
    return sorted(list, "title");
}

QVariantList Library::playlist(const QString &which) const
{
    QVariantList list;
    for (const QVariant &v : m_songs) {
        const QString url = v.toMap().value(QStringLiteral("url")).toString();
        if (which == QLatin1String("played") && !m_lastPlayed.contains(url))
            continue;
        if (which == QLatin1String("most") && !m_plays.contains(url))
            continue;
        list << v;
    }
    std::sort(list.begin(), list.end(), [&](const QVariant &a, const QVariant &b) {
        const QVariantMap x = a.toMap(), y = b.toMap();
        const QString ux = x.value(QStringLiteral("url")).toString(), uy = y.value(QStringLiteral("url")).toString();
        if (which == QLatin1String("played"))
            return m_lastPlayed.value(ux).toLongLong() > m_lastPlayed.value(uy).toLongLong();
        if (which == QLatin1String("most"))
            return m_plays.value(ux).toInt() > m_plays.value(uy).toInt();
        return x.value(QStringLiteral("added")).toLongLong() > y.value(QStringLiteral("added")).toLongLong();
    });
    return list.mid(0, 50);
}

QStringList Library::mosaic(int n) const
{
    QStringList covers;
    for (const QString &which : {QStringLiteral("played"), QStringLiteral("recent")}) {
        for (const QVariant &v : playlist(which)) {
            const QString c = v.toMap().value(QStringLiteral("cover")).toString();
            if (!c.isEmpty() && !covers.contains(c))
                covers << c;
            if (covers.size() >= n)
                return covers;
        }
    }
    return covers;
}
