// The player: a queue of songs on GStreamer's playbin, with shuffle and
// repeat; the bus and the position are polled from Qt's event loop.
#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

typedef struct _GstElement GstElement;

class Library;

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool playing READ playing NOTIFY stateChanged)
    Q_PROPERTY(bool active READ active NOTIFY currentChanged)
    Q_PROPERTY(QVariantMap current READ current NOTIFY currentChanged)
    Q_PROPERTY(int index READ index NOTIFY currentChanged)
    Q_PROPERTY(int count READ count NOTIFY currentChanged)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY positionChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY modeChanged)
    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY modeChanged)

public:
    explicit Player(Library *library, QObject *parent = nullptr);
    ~Player() override;

    bool playing() const { return m_playing; }
    bool active() const { return m_index >= 0; }
    QVariantMap current() const;
    int index() const { return m_index; }
    int count() const { return m_queue.size(); }
    qint64 position() const { return m_position; }
    qint64 duration() const { return m_duration; }
    bool shuffle() const { return m_shuffle; }
    bool repeat() const { return m_repeat; }
    void setShuffle(bool on);
    void setRepeat(bool on);

    Q_INVOKABLE void playQueue(const QVariantList &songs, int index);
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void seek(qint64 ms);

signals:
    void stateChanged();
    void currentChanged();
    void positionChanged();
    void modeChanged();

private:
    void start(int index);
    void setPlaying(bool on);
    void poll();
    int following() const;

    Library *m_library;
    GstElement *m_bin = nullptr;
    QTimer m_timer;
    QVariantList m_queue;
    QList<int> m_order;     // the play order (shuffled or not)
    int m_index = -1;       // into m_order
    bool m_playing = false, m_shuffle = false, m_repeat = false;
    qint64 m_position = 0, m_duration = 0;
};

#endif
