#include "videoitem.h"

#include <QPainter>

#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <gst/video/video.h>

VideoItem::VideoItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setFillColor(Qt::black);
    m_bin = gst_element_factory_make("playbin", nullptr);
    GstElement *sink = gst_element_factory_make("appsink", nullptr);
    if (m_bin && sink) {
        GstCaps *caps = gst_caps_from_string("video/x-raw,format=RGBx,pixel-aspect-ratio=1/1");
        g_object_set(sink, "caps", caps, "max-buffers", 2, "drop", TRUE, "sync", TRUE, nullptr);
        gst_caps_unref(caps);
        GstAppSinkCallbacks cb = {};
        cb.new_sample = reinterpret_cast<GstFlowReturn (*)(GstAppSink *, gpointer)>(&VideoItem::onSample);
        gst_app_sink_set_callbacks(GST_APP_SINK(sink), &cb, this, nullptr);
        // videoconvert and a square pixel scale in front of the sink
        GstElement *bin = gst_parse_bin_from_description("videoconvert ! videoscale", TRUE, nullptr);
        GstElement *video = gst_bin_new("video");
        gst_bin_add_many(GST_BIN(video), bin, sink, nullptr);
        gst_element_link(bin, sink);
        GstPad *pad = gst_element_get_static_pad(bin, "sink");
        gst_element_add_pad(video, gst_ghost_pad_new("sink", pad));
        gst_object_unref(pad);
        g_object_set(m_bin, "video-sink", video, nullptr);
        // another sound output (tests: "fakesink sync=true")
        const QByteArray audio = qgetenv("MEEGO_VIDEO_AUDIOSINK");
        if (!audio.isEmpty())
            if (GstElement *a = gst_parse_bin_from_description(audio.constData(), TRUE, nullptr))
                g_object_set(m_bin, "audio-sink", a, nullptr);
    }
    m_timer.setInterval(250);
    connect(&m_timer, &QTimer::timeout, this, &VideoItem::poll);
    // (started when there is something to look at: settle())
}

VideoItem::~VideoItem()
{
    if (m_bin) {
        gst_element_set_state(m_bin, GST_STATE_NULL);
        gst_object_unref(m_bin);
    }
}

// in GStreamer's streaming thread: the frame, then a repaint queued
int VideoItem::onSample(GstAppSink *sink, void *data)
{
    auto *self = static_cast<VideoItem *>(data);
    GstSample *sample = gst_app_sink_pull_sample(sink);
    if (!sample)
        return GST_FLOW_OK;
    GstVideoInfo info;
    GstBuffer *buf = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (gst_video_info_from_caps(&info, gst_sample_get_caps(sample)) && gst_buffer_map(buf, &map, GST_MAP_READ)) {
        const QImage img(map.data, GST_VIDEO_INFO_WIDTH(&info), GST_VIDEO_INFO_HEIGHT(&info),
                         GST_VIDEO_INFO_PLANE_STRIDE(&info, 0), QImage::Format_RGBX8888);
        {
            QMutexLocker l(&self->m_lock);
            self->m_frame = img.copy();
        }
        gst_buffer_unmap(buf, &map);
        QMetaObject::invokeMethod(self, "update", Qt::QueuedConnection);
    }
    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void VideoItem::paint(QPainter *painter)
{
    QMutexLocker l(&m_lock);
    if (m_frame.isNull())
        return;
    // the whole picture in the item, centred
    const QSizeF s = QSizeF(m_frame.size()).scaled(width(), height(), Qt::KeepAspectRatio);
    const QRectF r((width() - s.width()) / 2, (height() - s.height()) / 2, s.width(), s.height());
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawImage(r, m_frame);
}

void VideoItem::setSource(const QString &url)
{
    if (url == m_source || !m_bin)
        return;
    m_source = url;
    gst_element_set_state(m_bin, GST_STATE_NULL);
    {
        QMutexLocker l(&m_lock);
        m_frame = QImage();
    }
    m_error.clear();
    m_ended = false;
    m_position = m_duration = 0;
    g_object_set(m_bin, "uri", url.toUtf8().constData(), nullptr);
    gst_element_set_state(m_bin, GST_STATE_PAUSED);     // the first frame
    settle();
    emit sourceChanged();
    emit errorChanged();
    emit stateChanged();
    emit positionChanged();
    update();
}

void VideoItem::play()
{
    if (!m_bin || m_source.isEmpty())
        return;
    if (m_ended)
        seek(0);
    m_ended = false;
    gst_element_set_state(m_bin, GST_STATE_PLAYING);
    m_playing = true;
    settle();
    emit stateChanged();
}

void VideoItem::pause()
{
    if (!m_bin)
        return;
    gst_element_set_state(m_bin, GST_STATE_PAUSED);
    m_playing = false;
    settle();
    emit stateChanged();
}

void VideoItem::seek(qint64 ms)
{
    if (!m_bin)
        return;
    gst_element_seek_simple(m_bin, GST_FORMAT_TIME, GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                            ms * GST_MSECOND);
    m_position = ms;
    settle();
    emit positionChanged();
}

void VideoItem::settle()
{
    // two seconds more after the change, for its messages (the length,
    // an error), all the time while playing
    m_settle = 8;
    if (!m_timer.isActive())
        m_timer.start();
}

void VideoItem::poll()
{
    if (!m_bin)
        return;
    GstBus *bus = gst_element_get_bus(m_bin);
    while (GstMessage *msg = gst_bus_pop(bus)) {
        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS) {
            m_playing = false;
            m_ended = true;
            gst_element_set_state(m_bin, GST_STATE_PAUSED);
            emit stateChanged();
        } else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
            GError *err = nullptr;
            gst_message_parse_error(msg, &err, nullptr);
            m_error = QString::fromUtf8(err ? err->message : "playback error");
            g_clear_error(&err);
            m_playing = false;
            emit errorChanged();
            emit stateChanged();
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    gint64 pos = 0, dur = 0;
    bool changed = false;
    if (gst_element_query_position(m_bin, GST_FORMAT_TIME, &pos) && pos / GST_MSECOND != m_position) {
        m_position = pos / GST_MSECOND;
        changed = true;
    }
    if (gst_element_query_duration(m_bin, GST_FORMAT_TIME, &dur) && dur > 0 && dur / GST_MSECOND != m_duration) {
        m_duration = dur / GST_MSECOND;
        changed = true;
    }
    if (changed)
        emit positionChanged();
    if (!m_playing && --m_settle <= 0)
        m_timer.stop();
}
