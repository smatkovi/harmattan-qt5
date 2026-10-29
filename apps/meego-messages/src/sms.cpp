#include "sms.h"

#include <QCryptographicHash>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDateTime>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDebug>

static const char *MM = "org.freedesktop.ModemManager1";
static const char *MM_PATH = "/org/freedesktop/ModemManager1";
static const char *MESSAGING = "org.freedesktop.ModemManager1.Modem.Messaging";
static const char *SMSI = "org.freedesktop.ModemManager1.Sms";
static const char *PROPS = "org.freedesktop.DBus.Properties";

typedef QMap<QString, QVariantMap> InterfaceMap;
typedef QMap<QDBusObjectPath, InterfaceMap> ManagedObjects;
Q_DECLARE_METATYPE(InterfaceMap)
Q_DECLARE_METATYPE(ManagedObjects)

// MMSmsState
enum { Unknown, Stored, Receiving, Received, Sending, Sent };

static QDBusConnection busToUse()
{
    return qgetenv("MEEGO_MESSAGES_BUS") == "session" ? QDBusConnection::sessionBus()
                                                      : QDBusConnection::systemBus();
}

static QDBusMessage method(const QString &path, const char *iface, const char *name)
{
    return QDBusMessage::createMethodCall(QString::fromLatin1(MM), path, QString::fromLatin1(iface),
                                          QString::fromLatin1(name));
}

static QVariant prop(const QDBusConnection &bus, const QString &path, const char *iface, const char *name)
{
    QDBusMessage msg = method(path, PROPS, "Get");
    msg << QString::fromLatin1(iface) << QString::fromLatin1(name);
    QDBusReply<QDBusVariant> reply = bus.call(msg, QDBus::Block, 3000);
    return reply.isValid() ? reply.value().variant() : QVariant();
}

// the same number however it is written: digits and a leading +
static QString normalized(const QString &number)
{
    QString n;
    for (const QChar c : number)
        if (c.isDigit() || (c == QLatin1Char('+') && n.isEmpty()))
            n += c;
    return n.isEmpty() ? number : n;
}

Sms::Sms(QObject *parent) : QObject(parent), m_bus(busToUse())
{
    qDBusRegisterMetaType<InterfaceMap>();
    qDBusRegisterMetaType<ManagedObjects>();

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("sms"));
    m_db.setDatabaseName(dir + QStringLiteral("/messages.sqlite"));
    if (!m_db.open())
        qWarning() << "meego-messages: no database:" << m_db.lastError().text();
    QSqlQuery q(m_db);
    q.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          " number TEXT, peer TEXT, text TEXT, time INTEGER, incoming INTEGER, status TEXT,"
                          " unread INTEGER, key TEXT UNIQUE, path TEXT)"));

    m_bus.connect(QString::fromLatin1(MM), QString::fromLatin1(MM_PATH),
                  QStringLiteral("org.freedesktop.DBus.ObjectManager"), QStringLiteral("InterfacesAdded"),
                  this, SLOT(findModem()));
    auto *watcher = new QDBusServiceWatcher(QString::fromLatin1(MM), m_bus,
                                            QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged, this, [this] {
        m_modem.clear();
        emit availableChanged();
        findModem();
    });
    findModem();
}

void Sms::findModem()
{
    if (!m_modem.isEmpty())
        return;
    QDBusReply<ManagedObjects> reply = m_bus.call(
        method(QString::fromLatin1(MM_PATH), "org.freedesktop.DBus.ObjectManager", "GetManagedObjects"),
        QDBus::Block, 3000);
    if (!reply.isValid())
        return;
    const ManagedObjects objects = reply.value();
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it.value().contains(QString::fromLatin1(MESSAGING))) {
            m_modem = it.key().path();
            break;
        }
    }
    if (m_modem.isEmpty())
        return;
    m_bus.connect(QString::fromLatin1(MM), m_modem, QString::fromLatin1(MESSAGING), QStringLiteral("Added"),
                  this, SLOT(onAdded(QDBusObjectPath, bool)));
    emit availableChanged();
    // what the modem holds already (received while the app did not run)
    QDBusReply<QList<QDBusObjectPath>> list = m_bus.call(method(m_modem, MESSAGING, "List"), QDBus::Block, 5000);
    if (list.isValid())
        for (const QDBusObjectPath &p : list.value())
            import(p.path());
}

void Sms::onAdded(const QDBusObjectPath &path, bool received)
{
    if (received)
        import(path.path());
}

// A message the modem received: into the store once (its number, time
// and text are its key). One still arriving in parts comes when complete.
void Sms::import(const QString &path)
{
    const int state = prop(m_bus, path, SMSI, "State").toInt();
    if (state == Receiving) {
        m_bus.connect(QString::fromLatin1(MM), path, QString::fromLatin1(PROPS), QStringLiteral("PropertiesChanged"),
                      this, SLOT(onSmsState(QString, QVariantMap, QStringList)));
        return;
    }
    if (state != Received)
        return;
    const QString number = prop(m_bus, path, SMSI, "Number").toString();
    const QString text = prop(m_bus, path, SMSI, "Text").toString();
    const QString stamp = prop(m_bus, path, SMSI, "Timestamp").toString();
    QDateTime time = QDateTime::fromString(stamp, Qt::ISODate);
    if (!time.isValid())
        time = QDateTime::currentDateTime();
    const QString key = QString::fromLatin1(QCryptographicHash::hash(
        (number + QLatin1Char('|') + stamp + QLatin1Char('|') + text).toUtf8(), QCryptographicHash::Sha1).toHex());
    if (insert(number, text, time, true, QStringLiteral("received"), true, key) > 0)
        emit received(number, text);
}

void Sms::onSmsState(const QString &interface, const QVariantMap &changed, const QStringList &)
{
    if (interface != QLatin1String(SMSI) || !changed.contains(QStringLiteral("State")))
        return;
    const QString path = message().path();
    const int state = changed.value(QStringLiteral("State")).toInt();
    if (state == Received)
        import(path);
    else if (state == Sent)
        setStatus(path, QStringLiteral("sent"));
}

int Sms::insert(const QString &number, const QString &text, const QDateTime &time,
                bool incoming, const QString &status, bool unread, const QString &key)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("INSERT OR IGNORE INTO messages (number, peer, text, time, incoming, status,"
                             " unread, key) VALUES (?,?,?,?,?,?,?,?)"));
    q.addBindValue(number);
    q.addBindValue(normalized(number));
    q.addBindValue(text);
    q.addBindValue(time.toSecsSinceEpoch());
    q.addBindValue(incoming ? 1 : 0);
    q.addBindValue(status);
    q.addBindValue(unread ? 1 : 0);
    q.addBindValue(key.isEmpty() ? QVariant() : QVariant(key));
    if (!q.exec() || q.numRowsAffected() < 1)
        return 0;
    emit changed();
    return q.lastInsertId().toInt();
}

void Sms::setStatus(const QString &smsPath, const QString &status)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("UPDATE messages SET status=? WHERE path=?"));
    q.addBindValue(status);
    q.addBindValue(smsPath);
    q.exec();
    emit changed();
}

bool Sms::send(const QString &number, const QString &text)
{
    m_error.clear();
    if (number.trimmed().isEmpty() || text.isEmpty())
        return false;
    if (m_modem.isEmpty()) {
        m_error = tr("No modem");
        return false;
    }
    QDBusMessage create = method(m_modem, MESSAGING, "Create");
    create << QVariantMap{{QStringLiteral("number"), number.trimmed()}, {QStringLiteral("text"), text}};
    QDBusReply<QDBusObjectPath> reply = m_bus.call(create, QDBus::Block, 10000);
    if (!reply.isValid()) {
        m_error = reply.error().message();
        return false;
    }
    const QString path = reply.value().path();
    const int id = insert(number.trimmed(), text, QDateTime::currentDateTime(), false,
                          QStringLiteral("sending"), false, QString());
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("UPDATE messages SET path=? WHERE id=?"));
    q.addBindValue(path);
    q.addBindValue(id);
    q.exec();
    m_bus.connect(QString::fromLatin1(MM), path, QString::fromLatin1(PROPS), QStringLiteral("PropertiesChanged"),
                  this, SLOT(onSmsState(QString, QVariantMap, QStringList)));
    auto *call = new QDBusPendingCallWatcher(m_bus.asyncCall(method(path, SMSI, "Send"), 60000), this);
    connect(call, &QDBusPendingCallWatcher::finished, this, [this, path, call] {
        if (call->isError())
            setStatus(path, QStringLiteral("failed"));
        else
            setStatus(path, QStringLiteral("sent"));
        call->deleteLater();
    });
    return true;
}

int Sms::unread() const
{
    QSqlQuery q(m_db);
    q.exec(QStringLiteral("SELECT COUNT(*) FROM messages WHERE unread=1"));
    return q.next() ? q.value(0).toInt() : 0;
}

QVariantList Sms::conversations() const
{
    QVariantList list;
    QSqlQuery q(m_db);
    q.exec(QStringLiteral(
        "SELECT m.number, m.text, m.time, m.incoming, m.status, m.peer,"
        " (SELECT COUNT(*) FROM messages u WHERE u.peer=m.peer AND u.unread=1),"
        " (SELECT COUNT(*) FROM messages c WHERE c.peer=m.peer)"
        " FROM messages m WHERE m.id = (SELECT id FROM messages l WHERE l.peer=m.peer"
        " ORDER BY l.time DESC, l.id DESC LIMIT 1) ORDER BY m.time DESC"));
    while (q.next())
        list << QVariantMap{
            {QStringLiteral("number"), q.value(0)}, {QStringLiteral("text"), q.value(1)},
            {QStringLiteral("time"), QDateTime::fromSecsSinceEpoch(q.value(2).toLongLong())},
            {QStringLiteral("incoming"), q.value(3).toBool()}, {QStringLiteral("status"), q.value(4)},
            {QStringLiteral("peer"), q.value(5)}, {QStringLiteral("unread"), q.value(6)},
            {QStringLiteral("count"), q.value(7)}};
    return list;
}

QVariantList Sms::messages(const QString &number) const
{
    QVariantList list;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT id, text, time, incoming, status FROM messages WHERE peer=?"
                             " ORDER BY time, id"));
    q.addBindValue(normalized(number));
    q.exec();
    while (q.next())
        list << QVariantMap{
            {QStringLiteral("id"), q.value(0)}, {QStringLiteral("text"), q.value(1)},
            {QStringLiteral("time"), QDateTime::fromSecsSinceEpoch(q.value(2).toLongLong())},
            {QStringLiteral("incoming"), q.value(3).toBool()}, {QStringLiteral("status"), q.value(4)}};
    return list;
}

void Sms::markRead(const QString &number)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("UPDATE messages SET unread=0 WHERE peer=? AND unread=1"));
    q.addBindValue(normalized(number));
    q.exec();
    if (q.numRowsAffected() > 0)
        emit changed();
}

void Sms::removeConversation(const QString &number)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("DELETE FROM messages WHERE peer=?"));
    q.addBindValue(normalized(number));
    q.exec();
    emit changed();
}

void Sms::removeMessage(int id)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("DELETE FROM messages WHERE id=?"));
    q.addBindValue(id);
    q.exec();
    emit changed();
}
