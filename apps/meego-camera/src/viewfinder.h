// The camera on GStreamer: libcamera's source (libcamerasrc) where it is,
// else a V4L2 device; the picture through an appsink as RGB images into
// Qt Quick, a second appsink keeping the latest whole frame for a photo
// (a JPEG through gst_video_convert_sample); a video is the same pipeline
// with an encoding branch (H.264 in MP4, else VP8 in WebM) and the sound.
#ifndef VIEWFINDER_H
#define VIEWFINDER_H

#include <QElapsedTimer>
#include <QImage>
#include <QMutex>
#include <QQuickPaintedItem>
#include <QStringList>
#include <QTimer>

typedef struct _GstElement GstElement;
typedef struct _GstAppSink GstAppSink;

class Viewfinder : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool running READ running NOTIFY stateChanged)
    Q_PROPERTY(QString error READ error NOTIFY stateChanged)
    Q_PROPERTY(QStringList cameras READ cameras CONSTANT)
    Q_PROPERTY(int camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)
    Q_PROPERTY(int recordSeconds READ recordSeconds NOTIFY recordSecondsChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString last READ last NOTIFY lastChanged)
    Q_PROPERTY(bool lastIsVideo READ lastIsVideo NOTIFY lastChanged)
    Q_PROPERTY(QString folder READ folder CONSTANT)

public:
    explicit Viewfinder(QQuickItem *parent = nullptr);
    ~Viewfinder() override;

    bool active() const { return m_active; }
    void setActive(bool on);
    bool running() const { return m_running; }
    QString error() const { return m_error; }
    QStringList cameras() const { return m_names; }
    int camera() const { return m_camera; }
    void setCamera(int index);
    bool recording() const { return m_recording; }
    int recordSeconds() const { return m_recording ? int(m_clock.elapsed() / 1000) : 0; }
    bool busy() const { return m_busy; }
    QString last() const { return m_last; }
    bool lastIsVideo() const { return m_last.endsWith(QLatin1String(".mp4")) || m_last.endsWith(QLatin1String(".webm")); }
    QString folder() const { return m_dir; }

    Q_INVOKABLE void capture();
    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
    // the last picture or video in the gallery or the videos
    Q_INVOKABLE void review();

    void paint(QPainter *painter) override;

signals:
    void activeChanged();
    void stateChanged();
    void cameraChanged();
    void recordingChanged();
    void recordSecondsChanged();
    void busyChanged();
    void lastChanged();
    void captured(const QString &url);
    void failed(const QString &message);

private:
    static int onSample(GstAppSink *sink, void *self);   // GstFlowReturn
    void findCameras();
    void start(const QString &recordTo = QString());
    void stop();
    void poll();
    void setBusy(bool b);
    QString newName(const QString &suffix) const;
    QString recordBranch(const QString &file) const;

    GstElement *m_pipe = nullptr;
    GstElement *m_still = nullptr;
    QStringList m_names;       // what the user sees
    QStringList m_sources;     // gst-launch descriptions
    int m_camera = 0;
    bool m_active = false, m_running = false, m_recording = false, m_busy = false;
    bool m_stopping = false;   // the end of a video on its way
    QString m_error, m_dir, m_last, m_recordFile;
    QElapsedTimer m_clock, m_stopClock;
    QTimer m_timer;
    QMutex m_lock;
    QImage m_frame;
};

#endif
