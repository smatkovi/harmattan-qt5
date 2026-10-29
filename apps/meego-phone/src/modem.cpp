#include "modem.h"

#include <QDBusArgument>
#include <QDBusConnectionInterface>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QTimer>

static const char *MM = "org.freedesktop.ModemManager1";
static const char *MM_PATH = "/org/freedesktop/ModemManager1";
static const char *MODEM = "org.freedesktop.ModemManager1.Modem";
static const char *VOICE = "org.freedesktop.ModemManager1.Modem.Voice";
static const char *CALL = "org.freedesktop.ModemManager1.Call";
static const char *PROPS = "org.freedesktop.DBus.Properties";

typedef QMap<QString, QVariantMap> InterfaceMap;
typedef QMap<QDBusObjectPath, InterfaceMap> ManagedObjects;
Q_DECLARE_METATYPE(InterfaceMap)
Q_DECLARE_METATYPE(ManagedObjects)

// MMCallState
enum { Unknown, Dialing, RingingOut, RingingIn, Active, Held, Waiting, Terminated };

// A property through org.freedesktop.DBus.Properties (no introspection)
static QVariant prop(const QDBusConnection &bus, const QString &path, const char *iface, const char *name)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QString::fromLatin1(MM), path, QString::fromLatin1(PROPS),
                                                      QStringLiteral("Get"));
    msg << QString::fromLatin1(iface) << QString::fromLatin1(name);
    QDBusReply<QDBusVariant> reply = bus.call(msg, QDBus::Block, 3000);
    return reply.isValid() ? reply.value().variant() : QVariant();
}

static QDBusMessage method(const QString &path, const char *iface, const char *name)
{
    return QDBusMessage::createMethodCall(QString::fromLatin1(MM), path, QString::fromLatin1(iface),
                                          QString::fromLatin1(name));
}

static QDBusConnection busToUse()
{
    // tests talk to a stand-in on the session bus
    return qgetenv("MEEGO_PHONE_BUS") == "session" ? QDBusConnection::sessionBus()
                                                   : QDBusConnection::systemBus();
}

Modem::Modem(QObject *parent) : QObject(parent), m_bus(busToUse())
{
    qDBusRegisterMetaType<InterfaceMap>();
    qDBusRegisterMetaType<ManagedObjects>();
    m_bus.connect(QString::fromLatin1(MM), QString::fromLatin1(MM_PATH),
                  QStringLiteral("org.freedesktop.DBus.ObjectManager"), QStringLiteral("InterfacesAdded"),
                  this, SLOT(onInterfacesAdded(QDBusObjectPath, QVariantMap)));
    m_bus.connect(QString::fromLatin1(MM), QString::fromLatin1(MM_PATH),
                  QStringLiteral("org.freedesktop.DBus.ObjectManager"), QStringLiteral("InterfacesRemoved"),
                  this, SLOT(onInterfacesRemoved(QDBusObjectPath, QStringList)));
    auto *watcher = new QDBusServiceWatcher(QString::fromLatin1(MM), m_bus,
                                            QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged, this, [this] {
        m_modem.clear();
        endCall();
        emit modemChanged();
        findModem();
    });
    // the signal and the network change; ModemManager announces them, but
    // a look now and then costs nothing
    auto *timer = new QTimer(this);
    timer->setInterval(30000);
    connect(timer, &QTimer::timeout, this, &Modem::readModem);
    timer->start();
    findModem();
}

void Modem::findModem()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QString::fromLatin1(MM), QString::fromLatin1(MM_PATH),
                                                      QStringLiteral("org.freedesktop.DBus.ObjectManager"),
                                                      QStringLiteral("GetManagedObjects"));
    QDBusReply<ManagedObjects> reply = m_bus.call(msg, QDBus::Block, 3000);
    if (!reply.isValid())
        return;
    const ManagedObjects objects = reply.value();
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it.value().contains(QString::fromLatin1(VOICE))) {
            m_modem = it.key().path();
            break;
        }
    }
    if (m_modem.isEmpty())
        return;
    m_bus.connect(QString::fromLatin1(MM), m_modem, QString::fromLatin1(VOICE), QStringLiteral("CallAdded"),
                  this, SLOT(onCallAdded(QDBusObjectPath)));
    m_bus.connect(QString::fromLatin1(MM), m_modem, QString::fromLatin1(PROPS), QStringLiteral("PropertiesChanged"),
                  this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
    readModem();
    // a call that was there before this app (the call screen opened late)
    const QVariant calls = prop(m_bus, m_modem, VOICE, "Calls");
    const QList<QDBusObjectPath> list = qdbus_cast<QList<QDBusObjectPath>>(calls);
    if (!list.isEmpty())
        takeCall(list.first().path());
}

void Modem::readModem()
{
    if (m_modem.isEmpty())
        return;
    m_name = (prop(m_bus, m_modem, MODEM, "Manufacturer").toString() + QLatin1Char(' ')
              + prop(m_bus, m_modem, MODEM, "Model").toString()).trimmed();
    const QDBusArgument q = prop(m_bus, m_modem, MODEM, "SignalQuality").value<QDBusArgument>();
    if (q.currentType() == QDBusArgument::StructureType) {
        uint quality = 0;
        bool recent = false;
        q.beginStructure();
        q >> quality >> recent;
        q.endStructure();
        m_signal = int(quality);
    }
    m_operator = prop(m_bus, m_modem, "org.freedesktop.ModemManager1.Modem.Modem3gpp", "OperatorName").toString();
    emit modemChanged();
}

void Modem::onInterfacesAdded(const QDBusObjectPath &, const QVariantMap &)
{
    if (m_modem.isEmpty())
        findModem();
}

void Modem::onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &)
{
    if (path.path() == m_modem) {
        m_modem.clear();
        endCall();
        emit modemChanged();
    }
}

void Modem::onPropertiesChanged(const QString &interface, const QVariantMap &, const QStringList &)
{
    if (interface != QLatin1String(CALL))
        readModem();
}

QString Modem::callState() const
{
    if (m_call.isEmpty())
        return QString();
    switch (m_state) {
    case Dialing: return QStringLiteral("dialing");
    case RingingOut: return QStringLiteral("ringing-out");
    case RingingIn: return QStringLiteral("ringing-in");
    case Active: return QStringLiteral("active");
    case Held: return QStringLiteral("held");
    case Waiting: return QStringLiteral("waiting");
    case Terminated: return QStringLiteral("terminated");
    }
    return QStringLiteral("dialing");
}

void Modem::takeCall(const QString &path)
{
    if (!m_call.isEmpty() && m_call != path)
        return;  // one call at a time
    m_call = path;
    m_state = prop(m_bus, path, CALL, "State").toInt();
    m_callNumber = prop(m_bus, path, CALL, "Number").toString();
    m_incoming = prop(m_bus, path, CALL, "Direction").toInt() == 1;
    m_answered = m_state == Active;
    m_start = QDateTime::currentDateTime();
    m_activeSince = m_state == Active ? m_start : QDateTime();
    m_bus.connect(QString::fromLatin1(MM), path, QString::fromLatin1(CALL), QStringLiteral("StateChanged"),
                  this, SLOT(onCallStateChanged(int, int, uint)));
    emit callChanged();
}

void Modem::onCallAdded(const QDBusObjectPath &path)
{
    takeCall(path.path());
}

void Modem::onCallStateChanged(int, int newState, uint)
{
    m_state = newState;
    if (newState == Active && !m_activeSince.isValid()) {
        m_activeSince = QDateTime::currentDateTime();
        m_answered = true;
    }
    if (newState == Terminated) {
        endCall();
        return;
    }
    emit callChanged();
}

void Modem::endCall()
{
    if (m_call.isEmpty())
        return;
    const int seconds = m_activeSince.isValid() ? int(m_activeSince.secsTo(QDateTime::currentDateTime())) : 0;
    m_bus.disconnect(QString::fromLatin1(MM), m_call, QString::fromLatin1(CALL), QStringLiteral("StateChanged"),
                     this, SLOT(onCallStateChanged(int, int, uint)));
    // ModemManager keeps ended calls on the modem: take this one away
    if (!m_modem.isEmpty()) {
        QDBusMessage msg = method(m_modem, VOICE, "DeleteCall");
        msg << QVariant::fromValue(QDBusObjectPath(m_call));
        m_bus.asyncCall(msg);
    }
    emit callEnded(m_callNumber, m_incoming, m_answered, m_start, seconds);
    m_call.clear();
    m_state = Unknown;
    m_activeSince = QDateTime();
    emit callChanged();
}

bool Modem::dial(const QString &number)
{
    m_error.clear();
    if (m_modem.isEmpty()) {
        m_error = tr("No modem");
        return false;
    }
    if (number.trimmed().isEmpty() || inCall())
        return false;
    QDBusMessage create = method(m_modem, VOICE, "CreateCall");
    create << QVariantMap{{QStringLiteral("number"), number.trimmed()}};
    QDBusReply<QDBusObjectPath> reply = m_bus.call(create, QDBus::Block, 10000);
    if (!reply.isValid()) {
        m_error = reply.error().message();
        return false;
    }
    const QString path = reply.value().path();
    takeCall(path);
    m_incoming = false;
    QDBusReply<void> started = m_bus.call(method(path, CALL, "Start"), QDBus::Block, 10000);
    if (!started.isValid()) {
        m_error = started.error().message();
        endCall();
        return false;
    }
    emit callChanged();
    return true;
}

void Modem::accept()
{
    if (m_call.isEmpty())
        return;
    m_bus.asyncCall(method(m_call, CALL, "Accept"));
}

void Modem::hangup()
{
    if (m_call.isEmpty())
        return;
    QDBusReply<void> r = m_bus.call(method(m_call, CALL, "Hangup"), QDBus::Block, 5000);
    // the state change ends it; without one (an error, a modem gone) here
    if (!r.isValid())
        endCall();
}

void Modem::sendTone(const QString &tone)
{
    if (m_call.isEmpty() || m_state != Active)
        return;
    QDBusMessage msg = method(m_call, CALL, "SendDtmf");
    msg << tone;
    m_bus.asyncCall(msg);
}
