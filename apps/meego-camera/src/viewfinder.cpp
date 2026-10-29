#include "viewfinder.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QProcess>
#include <QUrl>
#include <QtConcurrent>

#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <gst/video/video.h>

static bool have(const char *factory)
{
    GstElementFactory *f = gst_element_factory_find(factory);
    if (f)
        gst_object_unref(f);
    return f;
}

Viewfinder::Viewfinder(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setFillColor(Qt::black);
    // where the N9 kept them: ~/MyDocs/DCIM, else ~/DCIM (the gallery's and
    // the videos' folders both)
    const QString home = QDir::homePath();
    m_dir = QDir(home + QStringLiteral("/MyDocs")).exists() ? home + QStringLiteral("/MyDocs/DCIM")
                                                             : home + QStringLiteral("/DCIM");
    if (qEnvironmentVariableIsSet("MEEGO_CAMERA_DIR"))
        m_dir = qEnvironmentVariable("MEEGO_CAMERA_DIR");
    const QFileInfoList files = QDir(m_dir).entryInfoList(
        {QStringLiteral("*.jpg"), QStringLiteral("*.mp4"), QStringLiteral("*.webm")}, QDir::Files, QDir::Time);
    if (!files.isEmpty())
        m_last = files.first().filePath();
    findCameras();
    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, &Viewfinder::poll);
}

Viewfinder::~Viewfinder()
{
    stop();
}

// the cameras: libcamera's where its source is there (on a phone the V4L2
// nodes are the sensors' raw ones), else the V4L2 devices; or the one in
// MEEGO_CAMERA_SOURCE (a test: "videotestsrc is-live=true")
void Viewfinder::findCameras()
{
    if (qEnvironmentVariableIsSet("MEEGO_CAMERA_SOURCE")) {
        m_names << QStringLiteral("Camera");
        m_sources << qEnvironmentVariable("MEEGO_CAMERA_SOURCE");
        return;
    }
    QStringList libcamera, libcameraNames, v4l2, v4l2Names;
    GstDeviceMonitor *mon = gst_device_monitor_new();
    gst_device_monitor_add_filter(mon, "Video/Source", nullptr);
    if (gst_device_monitor_start(mon)) {
        GList *devices = gst_device_monitor_get_devices(mon);
        for (GList *l = devices; l; l = l->next) {
            GstDevice *dev = GST_DEVICE(l->data);
            gchar *display = gst_device_get_display_name(dev);
            const QString name = QString::fromUtf8(display);
            g_free(display);
            if (GstElement *e = gst_device_create_element(dev, nullptr)) {
                const QByteArray factory = GST_OBJECT_NAME(gst_element_get_factory(e));
                if (factory == "libcamerasrc") {
                    gchar *id = nullptr;
                    g_object_get(e, "camera-name", &id, nullptr);
                    libcamera << QStringLiteral("libcamerasrc camera-name=\"%1\" ! video/x-raw,width=1280,height=720")
                                     .arg(QString::fromUtf8(id));
                    libcameraNames << name;
                    g_free(id);
                } else if (factory == "v4l2src") {
                    gchar *node = nullptr;
                    g_object_get(e, "device", &node, nullptr);
                    v4l2 << QStringLiteral("v4l2src device=%1").arg(QString::fromUtf8(node));
                    v4l2Names << name;
                    g_free(node);
                }
                gst_object_unref(e);
            }
        }
        g_list_free_full(devices, gst_object_unref);
        gst_device_monitor_stop(mon);
    }
    gst_object_unref(mon);
    if (!libcamera.isEmpty()) {
        m_sources = libcamera;
        m_names = libcameraNames;
    } else if (have("libcamerasrc") && v4l2.isEmpty()) {
        // a libcamera without its device provider: its first camera
        m_sources << QStringLiteral("libcamerasrc ! video/x-raw,width=1280,height=720");
        m_names << QStringLiteral("Camera");
    } else {
        m_sources = v4l2;
        m_names = v4l2Names;
    }
}

void Viewfinder::setActive(bool on)
{
    if (on == m_active)
        return;
    m_active = on;
    emit activeChanged();
    if (on)
        start();
    else
        stop();
}

void Viewfinder::setCamera(int index)
{
    if (index == m_camera || index < 0 || index >= m_sources.size() || m_recording)
        return;
    m_camera = index;
    emit cameraChanged();
    if (m_active) {
        stop();
        start();
    }
}

QString Viewfinder::newName(const QString &suffix) const
{
    QDir().mkpath(m_dir);
    const QString base = m_dir + QLatin1Char('/') + QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_hhmmss"));
    QString name = base + suffix;
    for (int n = 2; QFile::exists(name); ++n)
        name = base + QLatin1Char('_') + QString::number(n) + suffix;
    return name;
}

// the encoding branch for a video into FILE: H.264 in MP4 with AAC, else
// VP8 in WebM with Opus or Vorbis, as far as the plugins are there; the
// sound from the default input (MEEGO_CAMERA_AUDIOSRC another, "none" none)
QString Viewfinder::recordBranch(const QString &file) const
{
    QString video, mux, audio;
    const bool mp4 = file.endsWith(QLatin1String(".mp4"));
    if (mp4) {
        if (have("x264enc"))
            video = QStringLiteral("x264enc tune=zerolatency speed-preset=ultrafast bitrate=4000 key-int-max=60 ! h264parse");
        else if (have("openh264enc"))
            video = QStringLiteral("openh264enc ! h264parse");
        else
            video = QStringLiteral("v4l2h264enc ! video/x-h264,level=(string)4 ! h264parse");
        mux = QStringLiteral("mp4mux");
        if (have("avenc_aac"))
            audio = QStringLiteral("avenc_aac");
        else if (have("fdkaacenc"))
            audio = QStringLiteral("fdkaacenc");
        else if (have("voaacenc"))
            audio = QStringLiteral("voaacenc");
    } else {
        video = QStringLiteral("vp8enc deadline=1 cpu-used=8 target-bitrate=4000000");
        mux = QStringLiteral("webmmux");
        if (have("opusenc"))
            audio = QStringLiteral("opusenc");
        else if (have("vorbisenc"))
            audio = QStringLiteral("vorbisenc");
    }
    QString branch = QStringLiteral(" t. ! queue ! videoconvert ! video/x-raw,format=I420 ! %1 ! %2 name=mux ! filesink location=\"%3\"")
                         .arg(video, mux, file);
    QString source = qEnvironmentVariableIsSet("MEEGO_CAMERA_AUDIOSRC") ? qEnvironmentVariable("MEEGO_CAMERA_AUDIOSRC")
                                                                         : QStringLiteral("autoaudiosrc");
    if (!audio.isEmpty() && source != QLatin1String("none"))
        branch += QStringLiteral(" %1 ! queue ! audioconvert ! audioresample ! %2 ! mux.").arg(source, audio);
    return branch;
}

void Viewfinder::start(const QString &recordTo)
{
    if (m_pipe)
        return;
    if (m_sources.isEmpty()) {
        m_error = tr("No camera");
        emit stateChanged();
        return;
    }
    // the picture (at most 960 wide) for the screen; the whole frame kept
    // for a photo; a video's branch while one is made
    QString desc = m_sources.at(m_camera)
        + QStringLiteral(" ! decodebin ! videoconvert ! tee name=t"
                         " t. ! queue leaky=downstream max-size-buffers=1 ! videoscale"
                         " ! video/x-raw,format=RGBx,width=[16,960],pixel-aspect-ratio=1/1"
                         " ! appsink name=view max-buffers=1 drop=true sync=false"
                         " t. ! queue leaky=downstream max-size-buffers=1"
                         " ! appsink name=still max-buffers=1 drop=true sync=false");
    if (!recordTo.isEmpty())
        desc += recordBranch(recordTo);
    GError *err = nullptr;
    m_pipe = gst_parse_launch(desc.toUtf8().constData(), &err);
    if (err) {
        m_error = QString::fromUtf8(err->message);
        g_clear_error(&err);
        if (m_pipe) {
            gst_object_unref(m_pipe);
            m_pipe = nullptr;
        }
        emit stateChanged();
        return;
    }
    GstElement *view = gst_bin_get_by_name(GST_BIN(m_pipe), "view");
    GstAppSinkCallbacks cb = {};
    cb.new_sample = reinterpret_cast<GstFlowReturn (*)(GstAppSink *, gpointer)>(&Viewfinder::onSample);
    gst_app_sink_set_callbacks(GST_APP_SINK(view), &cb, this, nullptr);
    gst_object_unref(view);
    m_still = gst_bin_get_by_name(GST_BIN(m_pipe), "still");
    m_error.clear();
    if (gst_element_set_state(m_pipe, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
        m_error = tr("The camera does not start");
    m_timer.start();
    emit stateChanged();
}

void Viewfinder::stop()
{
    m_timer.stop();
    if (m_pipe) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        gst_object_unref(m_pipe);
        m_pipe = nullptr;
    }
    if (m_still) {
        gst_object_unref(m_still);
        m_still = nullptr;
    }
    m_running = false;
    m_stopping = false;
    emit stateChanged();
}

// in GStreamer's streaming thread: the frame, then a repaint queued
int Viewfinder::onSample(GstAppSink *sink, void *data)
{
    auto *self = static_cast<Viewfinder *>(data);
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

void Viewfinder::paint(QPainter *painter)
{
    QMutexLocker l(&m_lock);
    if (m_frame.isNull())
        return;
    const QSizeF s = QSizeF(m_frame.size()).scaled(width(), height(), Qt::KeepAspectRatio);
    const QRectF r((width() - s.width()) / 2, (height() - s.height()) / 2, s.width(), s.height());
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawImage(r, m_frame);
}

void Viewfinder::poll()
{
    if (!m_pipe)
        return;
    GstBus *bus = gst_element_get_bus(m_pipe);
    bool ended = false;
    while (GstMessage *msg = gst_bus_pop(bus)) {
        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_STATE_CHANGED:
            if (GST_MESSAGE_SRC(msg) == GST_OBJECT(m_pipe)) {
                GstState now;
                gst_message_parse_state_changed(msg, nullptr, &now, nullptr);
                if ((now == GST_STATE_PLAYING) != m_running) {
                    m_running = now == GST_STATE_PLAYING;
                    emit stateChanged();
                }
            }
            break;
        case GST_MESSAGE_EOS:
            ended = true;
            break;
        case GST_MESSAGE_ERROR: {
            GError *err = nullptr;
            gst_message_parse_error(msg, &err, nullptr);
            m_error = QString::fromUtf8(err ? err->message : "camera error");
            g_clear_error(&err);
            ended = true;
            break;
        }
        default:
            break;
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    if (m_recording)
        emit recordSecondsChanged();
    // a video's end: through, or no answer for five seconds
    if (m_stopping && !ended && m_stopClock.elapsed() > 5000)
        ended = true;
    if (!ended)
        return;
    const bool wasRecording = m_recording;
    const QString error = m_error;
    stop();
    m_error = error;
    if (wasRecording) {
        m_recording = false;
        emit recordingChanged();
        if (error.isEmpty() && QFileInfo(m_recordFile).size() > 0) {
            m_last = m_recordFile;
            emit lastChanged();
            emit captured(QUrl::fromLocalFile(m_last).toString());
        } else {
            QFile::remove(m_recordFile);
            emit failed(error.isEmpty() ? tr("The video could not be saved")
                                        : tr("The video could not be made: %1").arg(error));
        }
        setBusy(false);
        // the viewfinder again, whatever broke the video (its sound, its
        // encoder): the error was said in the banner
        m_error.clear();
        if (m_active)
            start();
    } else {
        emit stateChanged();
    }
}

void Viewfinder::setBusy(bool b)
{
    if (b != m_busy) {
        m_busy = b;
        emit busyChanged();
    }
}

void Viewfinder::capture()
{
    if (!m_still || m_busy || m_recording)
        return;
    GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(m_still), GST_SECOND / 2);
    if (!sample) {
        emit failed(tr("No picture from the camera"));
        return;
    }
    setBusy(true);
    const QString file = newName(QStringLiteral(".jpg"));
    // the JPEG away from the screen's thread
    QtConcurrent::run([this, sample, file] {
        GstCaps *caps = gst_caps_from_string("image/jpeg");
        GError *err = nullptr;
        GstSample *jpeg = gst_video_convert_sample(sample, caps, 10 * GST_SECOND, &err);
        gst_caps_unref(caps);
        gst_sample_unref(sample);
        bool ok = false;
        if (jpeg) {
            GstBuffer *buf = gst_sample_get_buffer(jpeg);
            GstMapInfo map;
            if (gst_buffer_map(buf, &map, GST_MAP_READ)) {
                QFile f(file);
                ok = f.open(QIODevice::WriteOnly) && f.write(reinterpret_cast<const char *>(map.data), map.size) == qint64(map.size);
                gst_buffer_unmap(buf, &map);
            }
            gst_sample_unref(jpeg);
        }
        g_clear_error(&err);
        QMetaObject::invokeMethod(this, [this, ok, file] {
            setBusy(false);
            if (!ok) {
                QFile::remove(file);
                emit failed(tr("The picture could not be saved"));
                return;
            }
            m_last = file;
            emit lastChanged();
            emit captured(QUrl::fromLocalFile(file).toString());
        }, Qt::QueuedConnection);
    });
}

void Viewfinder::startRecording()
{
    if (m_recording || m_busy || m_sources.isEmpty())
        return;
    const bool h264 = have("x264enc") || have("openh264enc") || have("v4l2h264enc");
    if (!h264 && !(have("vp8enc") && have("webmmux"))) {
        emit failed(tr("No video encoder (GStreamer's x264, OpenH264 or VP8)"));
        return;
    }
    m_recordFile = newName(h264 && have("mp4mux") ? QStringLiteral(".mp4") : QStringLiteral(".webm"));
    stop();
    start(m_recordFile);
    if (!m_pipe) {
        start();
        emit failed(tr("The video could not be started"));
        return;
    }
    m_recording = true;
    m_clock.start();
    emit recordingChanged();
}

void Viewfinder::stopRecording()
{
    if (!m_recording || m_stopping || !m_pipe)
        return;
    // the end through the pipeline, so that the file is whole
    m_stopping = true;
    setBusy(true);
    gst_element_send_event(m_pipe, gst_event_new_eos());
    m_stopClock.start();
}

void Viewfinder::review()
{
    if (m_last.isEmpty() || !QFile::exists(m_last))
        return;
    const QString app = lastIsVideo() ? QStringLiteral("meego-video") : QStringLiteral("meego-gallery");
    if (!QProcess::startDetached(app, {m_last}))
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_last));
}
