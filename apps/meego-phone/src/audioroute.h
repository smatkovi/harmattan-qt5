// The call's sound through PulseAudio (pactl): the microphone muted or
// not, the loudspeaker or the earpiece (the default sink's ports, when
// they have such names).
#ifndef AUDIOROUTE_H
#define AUDIOROUTE_H

#include <QObject>

class AudioRoute : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY changed)
    Q_PROPERTY(bool speaker READ speaker WRITE setSpeaker NOTIFY changed)
    Q_PROPERTY(bool canSwitchSpeaker READ canSwitchSpeaker NOTIFY changed)

public:
    explicit AudioRoute(QObject *parent = nullptr);

    bool muted() const { return m_muted; }
    bool speaker() const { return m_speaker; }
    bool canSwitchSpeaker() const { return !m_speakerPort.isEmpty() && !m_earpiecePort.isEmpty(); }
    void setMuted(bool on);
    void setSpeaker(bool on);

    // back to normal after a call
    Q_INVOKABLE void reset();
    Q_INVOKABLE void probe();

signals:
    void changed();

private:
    QString pactl(const QStringList &args) const;

    bool m_muted = false, m_speaker = false;
    QString m_sink, m_speakerPort, m_earpiecePort;
};

#endif
