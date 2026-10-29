// Text messages: the app's own store (SQLite) of the conversations, fed
// with what ModemManager's Messaging interface receives, and sending
// through it. Without ModemManager or a modem the store still shows.
#ifndef SMS_H
#define SMS_H

#include <QDBusConnection>
#include <QDBusContext>
#include <QDBusObjectPath>
#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>

class Sms : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(int unread READ unread NOTIFY changed)

public:
    explicit Sms(QObject *parent = nullptr);

    bool available() const { return !m_modem.isEmpty(); }
    int unread() const;

    // one per number, the newest first: number, text, time, incoming, unread
    Q_INVOKABLE QVariantList conversations() const;
    // a number's messages, oldest first: id, text, time, incoming, status
    // ("received", "sending", "sent", "failed")
    Q_INVOKABLE QVariantList messages(const QString &number) const;
    Q_INVOKABLE bool send(const QString &number, const QString &text);
    Q_INVOKABLE void markRead(const QString &number);
    Q_INVOKABLE void removeConversation(const QString &number);
    Q_INVOKABLE void removeMessage(int id);
    Q_INVOKABLE QString error() const { return m_error; }

signals:
    void changed();
    void availableChanged();
    void received(const QString &number, const QString &text);

private slots:
    void findModem();
    void onAdded(const QDBusObjectPath &path, bool received);
    void onSmsState(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);

private:
    void import(const QString &path);
    int insert(const QString &number, const QString &text, const QDateTime &time,
               bool incoming, const QString &status, bool unread, const QString &key);
    void setStatus(const QString &smsPath, const QString &status);

    QDBusConnection m_bus;
    QSqlDatabase m_db;
    QString m_modem, m_error;
};

#endif
