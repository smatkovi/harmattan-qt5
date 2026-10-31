#include "player.h"
#include "library.h"

#include <QRandomGenerator>
#include <algorithm>

#include <gst/gst.h>

Player::Player(Library *library, QObject *parent) : QObject(parent), m_library(library)
{
    m_bin = gst_element_factory_make("playbin", "player");
    // music only: no video window for a file that has a picture track
    if (m_bin) {
        gint flags = 0;
        g_object_get(m_bin, "flags", &flags, nullptr);
        g_object_set(m_bin, "flags", (flags | 0x2 /* audio */) & ~0x1 /* video */, nullptr);
        // another output (tests: "fakesink sync=true")
        const QByteArray sink = qgetenv("MEEGO_MUSIC_AUDIOSINK");
        if (!sink.isEmpty()) {
            if (GstElement *s = gst_parse_bin_from_description(sink.constData(), TRUE, nullptr))
                g_object_set(m_bin, "audio-sink", s, nullptr);
        }
    }
    m_timer.setInterval(250);
    connect(&m_timer, &QTimer::timeout, this, &Player::poll);
    // (started when a song plays: nothing to look at before)
}

Player::~Player()
{
    if (m_bin) {
        gst_element_set_state(m_bin, GST_STATE_NULL);
        gst_object_unref(m_bin);
    }
}

QVariantMap Player::current() const
{
    if (m_index < 0 || m_index >= m_order.size())
        return QVariantMap();
    return m_queue.value(m_order.at(m_index)).toMap();
}

void Player::playQueue(const QVariantList &songs, int index)
{
    m_queue = songs;
    m_order.clear();
    for (int i = 0; i < songs.size(); ++i)
        m_order << i;
    if (m_shuffle) {
        // the chosen song first, the rest in a random order
        m_order.removeAll(index);
        std::shuffle(m_order.begin(), m_order.end(), *QRandomGenerator::global());
        m_order.prepend(index);
        start(0);
    } else {
        start(index);
    }
}

void Player::start(int index)
{
    if (!m_bin || index < 0 || index >= m_order.size())
        return;
    m_index = index;
    const QVariantMap song = current();
    gst_element_set_state(m_bin, GST_STATE_READY);
    g_object_set(m_bin, "uri", song.value(QStringLiteral("url")).toString().toUtf8().constData(), nullptr);
    gst_element_set_state(m_bin, GST_STATE_PLAYING);
    m_position = 0;
    m_duration = song.value(QStringLiteral("duration")).toLongLong();
    setPlaying(true);
    m_library->played(song.value(QStringLiteral("url")).toString());
    emit currentChanged();
    emit positionChanged();
}

void Player::setPlaying(bool on)
{
    if (m_playing != on) {
        m_playing = on;
        emit stateChanged();
    }
    // looked at while playing; stopped a second after it stopped
    m_settle = 4;
    if (on && !m_timer.isActive())
        m_timer.start();
}

void Player::toggle()
{
    if (!m_bin || m_index < 0)
        return;
    gst_element_set_state(m_bin, m_playing ? GST_STATE_PAUSED : GST_STATE_PLAYING);
    setPlaying(!m_playing);
}

int Player::following() const
{
    if (m_index + 1 < m_order.size())
        return m_index + 1;
    return m_repeat ? 0 : -1;
}

void Player::next()
{
    const int n = following();
    if (n >= 0)
        start(n);
}

void Player::previous()
{
    // as on the N9: back to the start of the song, unless just started
    if (m_position > 3000 || m_index == 0) {
        seek(0);
        return;
    }
    start(m_index - 1);
}

void Player::seek(qint64 ms)
{
    if (!m_bin)
        return;
    gst_element_seek_simple(m_bin, GST_FORMAT_TIME,
                            GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), ms * GST_MSECOND);
    m_position = ms;
    emit positionChanged();
}

void Player::setShuffle(bool on)
{
    if (m_shuffle == on)
        return;
    m_shuffle = on;
    // the rest of the queue in a new order (or back in its own)
    if (m_index >= 0) {
        const int playing = m_order.at(m_index);
        if (on) {
            QList<int> rest = m_order.mid(m_index + 1);
            std::shuffle(rest.begin(), rest.end(), *QRandomGenerator::global());
            m_order = m_order.mid(0, m_index + 1) + rest;
        } else {
            m_order.clear();
            for (int i = 0; i < m_queue.size(); ++i)
                m_order << i;
            m_index = playing;
        }
    }
    emit modeChanged();
    emit currentChanged();
}

void Player::setRepeat(bool on)
{
    if (m_repeat != on) {
        m_repeat = on;
        emit modeChanged();
    }
}

void Player::poll()
{
    if (!m_bin)
        return;
    GstBus *bus = gst_element_get_bus(m_bin);
    while (GstMessage *msg = gst_bus_pop(bus)) {
        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS: {
            const int n = following();
            if (n >= 0) {
                start(n);
            } else {
                gst_element_set_state(m_bin, GST_STATE_READY);
                setPlaying(false);
                m_position = 0;
                emit positionChanged();
            }
            break;
        }
        case GST_MESSAGE_ERROR: {
            GError *err = nullptr;
            gst_message_parse_error(msg, &err, nullptr);
            qWarning("meego-music: %s", err ? err->message : "playback error");
            g_clear_error(&err);
            // the next song, as the N9 skips one it cannot play
            const int n = following();
            if (n >= 0 && n != m_index)
                start(n);
            else
                setPlaying(false);
            break;
        }
        default:
            break;
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    if (!m_playing && --m_settle <= 0)
        m_timer.stop();
    if (m_playing) {
        gint64 pos = 0, dur = 0;
        if (gst_element_query_position(m_bin, GST_FORMAT_TIME, &pos))
            m_position = pos / GST_MSECOND;
        if (gst_element_query_duration(m_bin, GST_FORMAT_TIME, &dur) && dur > 0)
            m_duration = dur / GST_MSECOND;
        emit positionChanged();
    }
}
