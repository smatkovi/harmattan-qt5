#include "smtp.h"

#include <QTimer>

Smtp::Smtp(QObject *parent) : QObject(parent)
{
    connect(&m_sock, &QSslSocket::readyRead, this, &Smtp::onReadyRead);
    connect(&m_sock, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this,
            [this](const QList<QSslError> &errors) {
                fail(tr("Secure connection failed: %1").arg(errors.value(0).errorString()));
            });
    connect(&m_sock, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        if (m_step != Idle && m_step != Quit)
            fail(m_sock.errorString());
    });
    connect(&m_sock, &QSslSocket::encrypted, this, [this] {
        if (m_step == StartTls) {
            m_step = Ehlo2;
            ehlo();
        }
    });
}

void Smtp::send(const Account &account, const QStringList &recipients, const QByteArray &message)
{
    if (busy()) {
        emit failed(tr("Busy"));
        return;
    }
    m_account = account;
    m_recipients = recipients;
    m_rcpt = 0;
    m_reply.clear();
    m_buf.clear();
    // dot-stuffing, CRLF line ends
    QByteArray body = message;
    body.replace("\r\n", "\n").replace("\n", "\r\n");
    body.replace("\r\n.", "\r\n..");
    if (body.startsWith('.'))
        body.prepend('.');
    m_message = body;
    m_step = Greeting;
    emit busyChanged();
    if (account.smtpSecurity == QLatin1String("ssl"))
        m_sock.connectToHostEncrypted(account.smtpHost, quint16(account.smtpPort));
    else
        m_sock.connectToHost(account.smtpHost, quint16(account.smtpPort));
    QTimer::singleShot(30000, this, [this] {
        if (m_step == Greeting)
            fail(tr("The mail server does not answer"));
    });
}

void Smtp::write(const QByteArray &line)
{
    m_sock.write(line + "\r\n");
}

void Smtp::fail(const QString &error)
{
    if (m_step == Idle)
        return;
    m_step = Idle;
    m_sock.abort();
    emit busyChanged();
    emit failed(error);
}

void Smtp::ehlo()
{
    write("EHLO meego-mail");
}

// Replies may span lines ("250-..." then "250 ...")
void Smtp::onReadyRead()
{
    m_buf += m_sock.readAll();
    int eol;
    while ((eol = m_buf.indexOf("\r\n")) >= 0) {
        const QByteArray line = m_buf.left(eol);
        m_buf.remove(0, eol + 2);
        m_reply += line.mid(4) + '\n';
        if (line.size() >= 4 && line.at(3) == '-')
            continue;
        const int code = line.left(3).toInt();
        const QByteArray text = m_reply;
        m_reply.clear();
        next(code, text);
    }
}

void Smtp::next(int code, const QByteArray &text)
{
    const QString err = QString::fromUtf8(text).trimmed();
    switch (m_step) {
    case Greeting:
        if (code != 220) { fail(err); return; }
        m_step = Ehlo;
        ehlo();
        break;
    case Ehlo:
    case Ehlo2:
        if (code != 250) { fail(err); return; }
        if (m_step == Ehlo && m_account.smtpSecurity == QLatin1String("starttls")) {
            m_step = StartTls;
            write("STARTTLS");
            break;
        }
        if (m_account.user.isEmpty()) {
            m_step = MailFrom;
            next(235, QByteArray());
            break;
        }
        m_authLogin = !text.toUpper().contains("PLAIN") && text.toUpper().contains("LOGIN");
        m_step = Auth;
        if (m_authLogin) {
            write("AUTH LOGIN");
        } else {
            write("AUTH PLAIN " + ('\0' + m_account.user.toUtf8() + '\0' + m_account.password.toUtf8()).toBase64());
        }
        break;
    case StartTls:
        if (code != 220) { fail(tr("No STARTTLS: %1").arg(err)); return; }
        m_sock.startClientEncryption();
        break;
    case Auth:
        if (m_authLogin && code == 334) {
            m_step = AuthUser;
            write(m_account.user.toUtf8().toBase64());
            break;
        }
        if (code != 235) { fail(tr("Login failed: %1").arg(err)); return; }
        m_step = MailFrom;
        next(235, QByteArray());
        break;
    case AuthUser:
        if (code != 334) { fail(tr("Login failed: %1").arg(err)); return; }
        m_step = AuthPass;
        write(m_account.password.toUtf8().toBase64());
        break;
    case AuthPass:
        if (code != 235) { fail(tr("Login failed: %1").arg(err)); return; }
        m_step = MailFrom;
        next(235, QByteArray());
        break;
    case MailFrom:
        m_step = RcptTo;
        write("MAIL FROM:<" + m_account.address.toUtf8() + ">");
        break;
    case RcptTo:
        if (code != 250 && code != 251) { fail(err); return; }
        if (m_rcpt < m_recipients.size()) {
            write("RCPT TO:<" + m_recipients.at(m_rcpt++).toUtf8() + ">");
            break;
        }
        m_step = Data;
        write("DATA");
        break;
    case Data:
        if (code != 354) { fail(err); return; }
        m_step = Body;
        m_sock.write(m_message);
        if (!m_message.endsWith("\r\n"))
            m_sock.write("\r\n");
        write(".");
        break;
    case Body:
        if (code != 250) { fail(err); return; }
        m_step = Quit;
        write("QUIT");
        m_step = Idle;
        m_sock.disconnectFromHost();
        emit busyChanged();
        emit sent();
        break;
    case Quit:
    case Idle:
        break;
    }
}
