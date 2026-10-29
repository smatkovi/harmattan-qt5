// Mail for the QML: the account, the folders, a folder's latest messages
// and a message's text, cached for reading without a network; sending.
#ifndef MAILSTORE_H
#define MAILSTORE_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

#include "imap.h"
#include "smtp.h"

class MailStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool configured READ configured NOTIFY accountChanged)
    Q_PROPERTY(QString address READ address NOTIFY accountChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool sending READ sending NOTIFY busyChanged)
    Q_PROPERTY(QString folder READ folder NOTIFY folderChanged)
    Q_PROPERTY(QString folderTitle READ folderTitle NOTIFY folderChanged)
    Q_PROPERTY(QVariantList folders READ folders NOTIFY foldersChanged)
    Q_PROPERTY(QVariantList messages READ messages NOTIFY messagesChanged)
    Q_PROPERTY(int unread READ unread NOTIFY messagesChanged)

public:
    explicit MailStore(QObject *parent = nullptr);

    bool configured() const { return !m_account.imapHost.isEmpty(); }
    QString address() const { return m_account.address; }
    bool busy() const { return m_imap.busy(); }
    bool sending() const { return m_smtp.busy(); }
    QString folder() const { return m_folder; }
    QString folderTitle() const;
    QVariantList folders() const;
    QVariantList messages() const { return m_messages; }
    int unread() const;

    // the servers of a known provider for an address, else imap./smtp.<domain>
    Q_INVOKABLE QVariantMap guess(const QString &address) const;
    Q_INVOKABLE QVariantMap account() const;
    Q_INVOKABLE void setup(const QVariantMap &account);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void openFolder(const QString &folder);
    // opened(uid, message) follows: subject, from, to, cc, date, text, attachments
    Q_INVOKABLE void open(uint uid);
    Q_INVOKABLE void markUnread(uint uid);
    Q_INVOKABLE void remove(uint uid);
    Q_INVOKABLE void send(const QString &to, const QString &cc, const QString &subject,
                          const QString &body, const QString &inReplyTo = QString());

signals:
    void accountChanged();
    void busyChanged();
    void folderChanged();
    void foldersChanged();
    void messagesChanged();
    void opened(uint uid, const QVariantMap &message);
    void sent();
    void failed(const QString &error);

private:
    void loadCache();
    void saveCache() const;
    void setSeenLocally(uint uid, bool seen);
    QString cacheFile(const QString &name) const;
    QVariantMap readMessage(uint uid, const QByteArray &raw) const;

    Account m_account;
    Imap m_imap;
    Smtp m_smtp;
    QString m_folder = QStringLiteral("INBOX");
    QStringList m_folders;
    QVariantList m_messages;
    QString m_cacheDir;
};

#endif
