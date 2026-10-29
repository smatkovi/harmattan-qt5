#include "audioroute.h"

#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

AudioRoute::AudioRoute(QObject *parent) : QObject(parent)
{
    probe();
}

QString AudioRoute::pactl(const QStringList &args) const
{
    // MEEGO_PHONE_NO_PACTL: leave the sound alone (tests on a PC)
    if (qEnvironmentVariableIsSet("MEEGO_PHONE_NO_PACTL")
        || QStandardPaths::findExecutable(QStringLiteral("pactl")).isEmpty())
        return QString();
    QProcess p;
    p.start(QStringLiteral("pactl"), args);
    if (!p.waitForFinished(2000))
        return QString();
    return QString::fromUtf8(p.readAllStandardOutput());
}

// The default sink's ports: one for the loudspeaker, one for the earpiece
// (handset) or headphones, as phones' UCM profiles name them
void AudioRoute::probe()
{
    m_sink = pactl({QStringLiteral("get-default-sink")}).trimmed();
    m_speakerPort.clear();
    m_earpiecePort.clear();
    if (m_sink.isEmpty())
        return;
    const QString all = pactl({QStringLiteral("list"), QStringLiteral("sinks")});
    const int at = all.indexOf(QStringLiteral("Name: ") + m_sink);
    if (at < 0)
        return;
    const int end = all.indexOf(QStringLiteral("\nSink #"), at);
    const QString sink = all.mid(at, end < 0 ? -1 : end - at);
    const QRegularExpression port(QStringLiteral("^\\s+([^\\s:]+): .*\\(.*priority"),
                                  QRegularExpression::MultilineOption);
    auto it = port.globalMatch(sink);
    while (it.hasNext()) {
        const QString name = it.next().captured(1);
        const QString l = name.toLower();
        if (l.contains(QLatin1String("speaker")))
            m_speakerPort = name;
        else if (l.contains(QLatin1String("earpiece")) || l.contains(QLatin1String("handset"))
                 || l.contains(QLatin1String("receiver")))
            m_earpiecePort = name;
    }
    emit changed();
}

void AudioRoute::setMuted(bool on)
{
    if (m_muted == on)
        return;
    m_muted = on;
    pactl({QStringLiteral("set-source-mute"), QStringLiteral("@DEFAULT_SOURCE@"),
           on ? QStringLiteral("1") : QStringLiteral("0")});
    emit changed();
}

void AudioRoute::setSpeaker(bool on)
{
    if (m_speaker == on || !canSwitchSpeaker())
        return;
    m_speaker = on;
    pactl({QStringLiteral("set-sink-port"), m_sink, on ? m_speakerPort : m_earpiecePort});
    emit changed();
}

void AudioRoute::reset()
{
    setMuted(false);
    setSpeaker(false);
}
