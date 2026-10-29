// A small IMAP4rev1 client: each job connects (TLS, STARTTLS or plain),
// logs in, runs its commands and logs out. Jobs: a folder's latest
// messages (with the folder list), one message whole, flags, deleting.
#ifndef IMAP_H
#define IMAP_H

#include <QObject>
#include <QSslSocket>
#include <QStringList>
#include <QVariantList>
#include <functional>

struct Account {
    QString name, address, user, password;
    QString imapHost, imapSecurity = QStringLiteral("ssl");   // ssl, starttls, none
    int imapPort = 993;
    QString smtpHost, smtpSecurity = QStringLiteral("ssl");
    int smtpPort = 465;
};

class Imap : public QObject
{
    Q_OBJECT

public:
    explicit Imap(QObject *parent = nullptr);

    bool busy() const { return m_busy; }

    // listed(folder, messages, folders): each message uid, subject, from,
    // fromAddress, to, date, seen, size, messageId
    void list(const Account &account, const QString &folder, int count);
    void fetch(const Account &account, const QString &folder, uint uid);
    void setSeen(const Account &account, const QString &folder, uint uid, bool seen);
    void remove(const Account &account, const QString &folder, uint uid);

    static QString folderName(const QByteArray &modifiedUtf7);

signals:
    void listed(const QString &folder, const QVariantList &messages, const QStringList &folders);
    void fetched(const QString &folder, uint uid, const QByteArray &raw);
    void done();
    void failed(const QString &error);
    void busyChanged();

private:
    typedef std::function<void(bool ok, const QList<QVariantList> &untagged, const QString &text)> Reply;
    typedef std::function<void()> Step;

    void start(const Account &account, Step afterLogin);
    void send(const QByteArray &command, Reply reply);
    void finish(const QString &error = QString());
    void onReadyRead();
    bool takeResponse(QByteArray *line, QList<QByteArray> *literals);
    static QVariantList tokenize(const QByteArray &line, const QList<QByteArray> &literals);
    static QByteArray quote(const QString &s);

    QSslSocket m_sock;
    QByteArray m_buf;
    QList<QVariantList> m_untagged;
    QByteArray m_tag;
    Reply m_reply;
    Step m_afterLogin;
    int m_seq = 0;
    bool m_busy = false, m_greeted = false;
    Account m_account;
};

#endif
