#include "videolibrary.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImage>
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>
#include <algorithm>

#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <gst/pbutils/pbutils.h>
#include <gst/video/video.h>

// A frame a tenth into the video, 320 wide, as a JPEG in the cache;
// its length. Done once per file (the file's time is in the name).
static QString thumbnail(const QString &path, const QString &out, qint64 *duration)
{
    if (QFile::exists(out)) {
        QFile meta(out + QStringLiteral(".ms"));
        if (meta.open(QIODevice::ReadOnly))
            *duration = meta.readAll().toLongLong();
        return out;
    }
    const QByteArray uri = QUrl::fromLocalFile(path).toEncoded();
    const QByteArray desc = "uridecodebin uri=\"" + uri + "\" ! videoconvert ! videoscale ! "
        "video/x-raw,format=RGBx,width=320,pixel-aspect-ratio=1/1 ! appsink name=sink sync=false";
    GError *err = nullptr;
    GstElement *pipe = gst_parse_launch(desc.constData(), &err);
    if (!pipe) {
        g_clear_error(&err);
        return QString();
    }
    GstElement *sink = gst_bin_get_by_name(GST_BIN(pipe), "sink");
    QString result;
    gst_element_set_state(pipe, GST_STATE_PAUSED);
    if (gst_element_get_state(pipe, nullptr, nullptr, 10 * GST_SECOND) != GST_STATE_CHANGE_FAILURE) {
        gint64 dur = 0;
        if (gst_element_query_duration(pipe, GST_FORMAT_TIME, &dur) && dur > 0) {
            *duration = dur / GST_MSECOND;
            // accurate: a key frame may be far before the tenth
            gst_element_seek_simple(pipe, GST_FORMAT_TIME, GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE), dur / 10);
            gst_element_get_state(pipe, nullptr, nullptr, 10 * GST_SECOND);
        }
        if (GstSample *sample = gst_app_sink_try_pull_preroll(GST_APP_SINK(sink), 5 * GST_SECOND)) {
            GstVideoInfo info;
            GstBuffer *buf = gst_sample_get_buffer(sample);
            GstMapInfo map;
            if (gst_video_info_from_caps(&info, gst_sample_get_caps(sample)) && gst_buffer_map(buf, &map, GST_MAP_READ)) {
                QImage img(map.data, GST_VIDEO_INFO_WIDTH(&info), GST_VIDEO_INFO_HEIGHT(&info),
                           GST_VIDEO_INFO_PLANE_STRIDE(&info, 0), QImage::Format_RGBX8888);
                if (img.save(out, "JPEG", 85))
                    result = out;
                gst_buffer_unmap(buf, &map);
            }
            gst_sample_unref(sample);
        }
    }
    gst_element_set_state(pipe, GST_STATE_NULL);
    gst_object_unref(sink);
    gst_object_unref(pipe);
    QFile meta(out + QStringLiteral(".ms"));
    if (meta.open(QIODevice::WriteOnly))
        meta.write(QByteArray::number(*duration));
    return result;
}

static QVariantList scan(const QStringList &roots, const QString &cacheDir)
{
    QVariantList list;
    QSet<QString> seen;
    const QStringList filters{QStringLiteral("*.mp4"), QStringLiteral("*.m4v"), QStringLiteral("*.mkv"),
                              QStringLiteral("*.webm"), QStringLiteral("*.avi"), QStringLiteral("*.mov"),
                              QStringLiteral("*.ogv"), QStringLiteral("*.3gp"), QStringLiteral("*.MP4"),
                              QStringLiteral("*.MOV")};
    QDir().mkpath(cacheDir);
    for (const QString &root : roots) {
        QDirIterator it(root, filters, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QFileInfo fi(it.next());
            const QString path = fi.canonicalFilePath();
            if (path.isEmpty() || seen.contains(path) || path.contains(QLatin1String("/.")))
                continue;
            seen.insert(path);
            const QString key = QString::fromLatin1(QCryptographicHash::hash(
                (path + QString::number(fi.lastModified().toSecsSinceEpoch())).toUtf8(), QCryptographicHash::Md5).toHex());
            qint64 duration = 0;
            const QString thumb = thumbnail(path, cacheDir + QLatin1Char('/') + key + QStringLiteral(".jpg"), &duration);
            list << QVariantMap{{QStringLiteral("url"), QUrl::fromLocalFile(path).toString()},
                                {QStringLiteral("path"), path},
                                {QStringLiteral("title"), fi.completeBaseName()},
                                {QStringLiteral("time"), fi.lastModified()},
                                {QStringLiteral("duration"), duration},
                                {QStringLiteral("thumbnail"), thumb.isEmpty() ? QString() : QUrl::fromLocalFile(thumb).toString()}};
        }
    }
    std::sort(list.begin(), list.end(), [](const QVariant &a, const QVariant &b) {
        return a.toMap().value(QStringLiteral("time")).toDateTime() > b.toMap().value(QStringLiteral("time")).toDateTime();
    });
    return list;
}

VideoLibrary::VideoLibrary(QObject *parent) : QObject(parent)
{
    const QString home = QDir::homePath();
    m_roots << QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
            << home + QStringLiteral("/DCIM") << home + QStringLiteral("/MyDocs/DCIM");
    if (qEnvironmentVariableIsSet("MEEGO_VIDEO_DIR"))
        m_roots = qEnvironmentVariable("MEEGO_VIDEO_DIR").split(QLatin1Char(':'));
    m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/thumbnails");
    connect(&m_watcher, &QFutureWatcher<QVariantList>::finished, this, [this] {
        m_videos = m_watcher.result();
        emit scanningChanged();
        emit changed();
    });
    rescan();
}

void VideoLibrary::rescan()
{
    if (m_watcher.isRunning())
        return;
    m_watcher.setFuture(QtConcurrent::run(scan, m_roots, m_cacheDir));
    emit scanningChanged();
}

bool VideoLibrary::remove(const QString &path)
{
    QFile f(path);
    if (!f.moveToTrash() && !f.remove())
        return false;
    for (int i = 0; i < m_videos.size(); ++i)
        if (m_videos.at(i).toMap().value(QStringLiteral("path")).toString() == path)
            m_videos.removeAt(i--);
    emit changed();
    return true;
}
