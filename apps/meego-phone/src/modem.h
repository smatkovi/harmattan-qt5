// The modem through ModemManager's D-Bus interface: the first modem with
// voice calls, its network, and one call at a time (dialled or coming
// in). Without ModemManager or a modem, available is false.
#ifndef MODEM_H
#define MODEM_H

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDateTime>
#include <QObject>
#include <QVariantMap>

class Modem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY modemChanged)
    Q_PROPERTY(QString name READ name NOTIFY modemChanged)
    Q_PROPERTY(QString operatorName READ operatorName NOTIFY modemChanged)
    Q_PROPERTY(int signal READ signal NOTIFY modemChanged)
    Q_PROPERTY(bool inCall READ inCall NOTIFY callChanged)
    Q_PROPERTY(QString callState READ callState NOTIFY callChanged)
    Q_PROPERTY(QString callNumber READ callNumber NOTIFY callChanged)
    Q_PROPERTY(bool incoming READ incoming NOTIFY callChanged)
    Q_PROPERTY(QDateTime callActiveSince READ callActiveSince NOTIFY callChanged)

public:
    explicit Modem(QObject *parent = nullptr);

    bool available() const { return !m_modem.isEmpty(); }
    QString name() const { return m_name; }
    QString operatorName() const { return m_operator; }
    int signal() const { return m_signal; }
    bool inCall() const { return !m_call.isEmpty(); }
    // "dialing", "ringing-out", "ringing-in", "active", "held", "waiting",
    // "terminated", or "" without a call
    QString callState() const;
    QString callNumber() const { return m_callNumber; }
    bool incoming() const { return m_incoming; }
    QDateTime callActiveSince() const { return m_activeSince; }

    // false when there is no modem, the number is empty, or ModemManager
    // refused the call (error() says why)
    Q_INVOKABLE bool dial(const QString &number);
    Q_INVOKABLE void accept();
    Q_INVOKABLE void hangup();
    Q_INVOKABLE void sendTone(const QString &tone);
    Q_INVOKABLE QString error() const { return m_error; }

signals:
    void modemChanged();
    void callChanged();
    // a call ended: for the history
    void callEnded(const QString &number, bool incoming, bool answered,
                   const QDateTime &start, int seconds);

private slots:
    void findModem();
    void onInterfacesAdded(const QDBusObjectPath &path, const QVariantMap &interfaces);
    void onInterfacesRemoved(const QDBusObjectPath &path, const QStringList &interfaces);
    void onCallAdded(const QDBusObjectPath &path);
    void onCallStateChanged(int oldState, int newState, uint reason);
    void onPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);
    void readModem();

private:
    void takeCall(const QString &path);
    void endCall();

    QDBusConnection m_bus;
    QString m_modem, m_call, m_name, m_operator, m_callNumber, m_error;
    int m_signal = 0;
    int m_state = 0;
    bool m_incoming = false, m_answered = false;
    QDateTime m_start, m_activeSince;
};

#endif
