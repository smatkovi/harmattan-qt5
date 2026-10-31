// A video on GStreamer's playbin, drawn by Qt Quick: the frames come
// through an appsink as RGB images (no GL needed, as on a device whose
// GL GStreamer does not reach), the sound to the default output.
#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QImage>
#include <QMutex>
#include <QQuickPaintedItem>
#include <QTimer>

typedef struct _GstElement GstElement;
typedef struct _GstAppSink GstAppSink;

class VideoItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY stateChanged)
    Q_PROPERTY(bool ended READ ended NOTIFY stateChanged)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY positionChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:
    explicit VideoItem(QQuickItem *parent = nullptr);
    ~VideoItem() override;

    QString source() const { return m_source; }
    void setSource(const QString &url);
    bool playing() const { return m_playing; }
    bool ended() const { return m_ended; }
    qint64 position() const { return m_position; }
    qint64 duration() const { return m_duration; }
    QString error() const { return m_error; }

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void toggle() { m_playing ? pause() : play(); }
    Q_INVOKABLE void seek(qint64 ms);

    void paint(QPainter *painter) override;

signals:
    void sourceChanged();
    void stateChanged();
    void positionChanged();
    void errorChanged();

private:
    static int onSample(GstAppSink *sink, void *self);   // GstFlowReturn
    void poll();

    GstElement *m_bin = nullptr;
    // GStreamer's messages and the position, while playing and a moment
    // after a change (the first frame, a seek, pausing)
    QTimer m_timer;
    int m_settle = 0;
    void settle();
    QMutex m_lock;
    QImage m_frame;
    QString m_source, m_error;
    bool m_playing = false, m_ended = false;
    qint64 m_position = 0, m_duration = 0;
};

#endif
