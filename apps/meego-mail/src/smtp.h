// Sending a message over SMTP: TLS, STARTTLS or plain, AUTH PLAIN or
// LOGIN, one message per connection.
#ifndef SMTP_H
#define SMTP_H

#include <QObject>
#include <QSslSocket>
#include <QStringList>

#include "imap.h"

class Smtp : public QObject
{
    Q_OBJECT

public:
    explicit Smtp(QObject *parent = nullptr);

    bool busy() const { return m_step != Idle; }
    void send(const Account &account, const QStringList &recipients, const QByteArray &message);

signals:
    void sent();
    void failed(const QString &error);
    void busyChanged();

private:
    enum Step { Idle, Greeting, Ehlo, StartTls, Ehlo2, Auth, AuthUser, AuthPass, MailFrom, RcptTo, Data, Body, Quit };

    void onReadyRead();
    void next(int code, const QByteArray &text);
    void write(const QByteArray &line);
    void fail(const QString &error);
    void ehlo();

    QSslSocket m_sock;
    QByteArray m_buf, m_message, m_reply;
    QStringList m_recipients;
    int m_rcpt = 0;
    bool m_authLogin = false;
    Step m_step = Idle;
    Account m_account;
};

#endif
