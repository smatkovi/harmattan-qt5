#include "imap.h"
#include "mime.h"

#include <QDateTime>
#include <QTimer>

Imap::Imap(QObject *parent) : QObject(parent)
{
    connect(&m_sock, &QSslSocket::readyRead, this, &Imap::onReadyRead);
    connect(&m_sock, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this,
            [this](const QList<QSslError> &errors) {
                finish(tr("Secure connection failed: %1").arg(errors.value(0).errorString()));
            });
    connect(&m_sock, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        if (m_busy)
            finish(m_sock.errorString());
    });
}

QByteArray Imap::quote(const QString &s)
{
    QByteArray b = s.toUtf8();
    b.replace('\\', "\\\\").replace('"', "\\\"");
    return '"' + b + '"';
}

// IMAP's modified UTF-7 (RFC 3501 5.1.3): &...- is UTF-16 in base64 with ','
QString Imap::folderName(const QByteArray &m)
{
    QString out;
    for (int i = 0; i < m.size(); ++i) {
        if (m.at(i) != '&') {
            out += QLatin1Char(m.at(i));
            continue;
        }
        const int end = m.indexOf('-', i);
        if (end < 0)
            break;
        if (end == i + 1) {
            out += QLatin1Char('&');
        } else {
            QByteArray b64 = m.mid(i + 1, end - i - 1).replace(',', '/');
            while (b64.size() % 4)
                b64 += '=';
            const QByteArray u16 = QByteArray::fromBase64(b64);
            for (int k = 0; k + 1 < u16.size(); k += 2)
                out += QChar(ushort((uchar(u16.at(k)) << 8) | uchar(u16.at(k + 1))));
        }
        i = end;
    }
    return out;
}

void Imap::start(const Account &account, Step afterLogin)
{
    if (m_busy) {
        emit failed(tr("Busy"));
        return;
    }
    m_account = account;
    m_afterLogin = afterLogin;
    m_busy = true;
    m_greeted = false;
    m_buf.clear();
    m_untagged.clear();
    m_reply = nullptr;
    emit busyChanged();
    if (account.imapSecurity == QLatin1String("ssl"))
        m_sock.connectToHostEncrypted(account.imapHost, quint16(account.imapPort));
    else
        m_sock.connectToHost(account.imapHost, quint16(account.imapPort));
    // a server that does not answer
    QTimer::singleShot(30000, this, [this] {
        if (m_busy && !m_greeted)
            finish(tr("The mail server does not answer"));
    });
}

void Imap::send(const QByteArray &command, Reply reply)
{
    m_tag = "A" + QByteArray::number(++m_seq);
    m_reply = reply;
    m_untagged.clear();
    m_sock.write(m_tag + ' ' + command + "\r\n");
}

void Imap::finish(const QString &error)
{
    if (!m_busy)
        return;
    m_busy = false;
    m_reply = nullptr;
    if (m_sock.state() == QAbstractSocket::ConnectedState)
        m_sock.write("Z LOGOUT\r\n");
    m_sock.disconnectFromHost();
    m_sock.abort();
    emit busyChanged();
    if (error.isEmpty())
        emit done();
    else
        emit failed(error);
}

// One whole response (a line with its literals) from the buffer; a literal
// {n} goes into the list and the line gets \x01<index> in its place
bool Imap::takeResponse(QByteArray *line, QList<QByteArray> *literals)
{
    int pos = 0;
    QByteArray out;
    QList<QByteArray> lits;
    for (;;) {
        const int eol = m_buf.indexOf("\r\n", pos);
        if (eol < 0)
            return false;
        QByteArray part = m_buf.mid(pos, eol - pos);
        if (part.endsWith('}')) {
            const int brace = part.lastIndexOf('{');
            bool ok = false;
            const int n = part.mid(brace + 1, part.size() - brace - 2).replace("+", "").toInt(&ok);
            if (brace >= 0 && ok) {
                if (m_buf.size() < eol + 2 + n)
                    return false;
                out += part.left(brace) + '\x01' + QByteArray::number(lits.size()) + ' ';
                lits << m_buf.mid(eol + 2, n);
                pos = eol + 2 + n;
                continue;
            }
        }
        out += part;
        m_buf.remove(0, eol + 2);
        *line = out;
        *literals = lits;
        return true;
    }
}

QVariantList Imap::tokenize(const QByteArray &s, const QList<QByteArray> &literals)
{
    QList<QVariantList> stack;
    stack << QVariantList();
    int i = 0;
    while (i < s.size()) {
        const char c = s.at(i);
        if (c == ' ') {
            ++i;
        } else if (c == '(') {
            stack << QVariantList();
            ++i;
        } else if (c == ')') {
            if (stack.size() > 1) {
                const QVariantList l = stack.takeLast();
                stack.last() << QVariant(l);
            }
            ++i;
        } else if (c == '"') {
            QByteArray v;
            ++i;
            while (i < s.size() && s.at(i) != '"') {
                if (s.at(i) == '\\' && i + 1 < s.size())
                    ++i;
                v += s.at(i++);
            }
            ++i;
            stack.last() << QVariant(v);
        } else if (c == '\x01') {
            int j = i + 1;
            while (j < s.size() && s.at(j) >= '0' && s.at(j) <= '9')
                ++j;
            stack.last() << QVariant(literals.value(s.mid(i + 1, j - i - 1).toInt()));
            i = j;
        } else {
            // an atom; brackets (BODY[...]) belong to it
            int j = i;
            int depth = 0;
            while (j < s.size()) {
                const char d = s.at(j);
                if (d == '[') ++depth;
                else if (d == ']') --depth;
                else if (depth == 0 && (d == ' ' || d == '(' || d == ')'))
                    break;
                ++j;
            }
            const QByteArray atom = s.mid(i, j - i);
            stack.last() << (atom.toUpper() == "NIL" ? QVariant() : QVariant(atom));
            i = j;
        }
    }
    while (stack.size() > 1) {
        const QVariantList l = stack.takeLast();
        stack.last() << QVariant(l);
    }
    return stack.first();
}

void Imap::onReadyRead()
{
    m_buf += m_sock.readAll();
    QByteArray line;
    QList<QByteArray> literals;
    while (takeResponse(&line, &literals)) {
        if (!m_greeted) {
            m_greeted = true;
            if (!line.startsWith("* OK") && !line.startsWith("* PREAUTH")) {
                finish(tr("The mail server refused: %1").arg(QString::fromUtf8(line)));
                return;
            }
            auto login = [this] {
                send("LOGIN " + quote(m_account.user) + ' ' + quote(m_account.password),
                     [this](bool ok, const QList<QVariantList> &, const QString &text) {
                         if (!ok) {
                             finish(tr("Login failed: %1").arg(text));
                             return;
                         }
                         m_afterLogin();
                     });
            };
            if (m_account.imapSecurity == QLatin1String("starttls")) {
                send("STARTTLS", [this, login](bool ok, const QList<QVariantList> &, const QString &text) {
                    if (!ok) {
                        finish(tr("No STARTTLS: %1").arg(text));
                        return;
                    }
                    // once (Qt 5 has no single-shot connections)
                    auto *c = new QMetaObject::Connection;
                    *c = connect(&m_sock, &QSslSocket::encrypted, this, [this, login, c] {
                        disconnect(*c);
                        delete c;
                        login();
                    });
                    m_sock.startClientEncryption();
                });
            } else {
                login();
            }
            continue;
        }
        if (line.startsWith("* ")) {
            m_untagged << tokenize(line.mid(2), literals);
            continue;
        }
        if (line.startsWith("+")) {
            continue;
        }
        if (!m_tag.isEmpty() && line.startsWith(m_tag + ' ')) {
            const QByteArray rest = line.mid(m_tag.size() + 1);
            const bool ok = rest.startsWith("OK");
            const QString text = QString::fromUtf8(rest.mid(rest.indexOf(' ') + 1));
            Reply r = m_reply;
            const QList<QVariantList> untagged = m_untagged;
            m_reply = nullptr;
            m_untagged.clear();
            if (r)
                r(ok, untagged, text);
        }
    }
}

static QString str(const QVariant &v)
{
    return v.isNull() ? QString() : Mime::decodeWords(v.toByteArray());
}

// ((name adl mailbox host) ...) -> "Name <a@b>" and the first address
static QString addresses(const QVariant &v, QString *first = nullptr)
{
    QStringList out;
    for (const QVariant &a : v.toList()) {
        const QVariantList l = a.toList();
        if (l.size() < 4)
            continue;
        const QString addr = QString::fromUtf8(l.at(2).toByteArray()) + QLatin1Char('@')
                             + QString::fromUtf8(l.at(3).toByteArray());
        const QString name = str(l.at(0));
        if (first && first->isEmpty())
            *first = addr;
        out << (name.isEmpty() ? addr : name + QStringLiteral(" <") + addr + QLatin1Char('>'));
    }
    return out.join(QStringLiteral(", "));
}

void Imap::list(const Account &account, const QString &folder, int count)
{
    start(account, [this, folder, count] {
        send("LIST \"\" \"*\"", [this, folder, count](bool, const QList<QVariantList> &untagged, const QString &) {
            QStringList folders;
            for (const QVariantList &u : untagged)
                if (u.value(0).toByteArray().toUpper() == "LIST")
                    folders << QString::fromLatin1(u.value(3).toByteArray());
            send("SELECT " + quote(folder), [this, folder, count, folders](bool ok, const QList<QVariantList> &, const QString &text) {
                if (!ok) {
                    finish(tr("No folder %1: %2").arg(folder, text));
                    return;
                }
                send("UID SEARCH ALL", [this, folder, count, folders](bool ok, const QList<QVariantList> &untagged, const QString &text) {
                    if (!ok) {
                        finish(text);
                        return;
                    }
                    QList<uint> uids;
                    for (const QVariantList &u : untagged)
                        if (u.value(0).toByteArray().toUpper() == "SEARCH")
                            for (int i = 1; i < u.size(); ++i)
                                uids << u.at(i).toByteArray().toUInt();
                    std::sort(uids.begin(), uids.end());
                    uids = uids.mid(qMax(0, uids.size() - count));
                    if (uids.isEmpty()) {
                        emit listed(folder, QVariantList(), folders);
                        finish();
                        return;
                    }
                    QByteArrayList set;
                    for (uint u : uids)
                        set << QByteArray::number(u);
                    send("UID FETCH " + set.join(',') + " (UID FLAGS INTERNALDATE RFC822.SIZE ENVELOPE)",
                         [this, folder, folders](bool ok, const QList<QVariantList> &untagged, const QString &text) {
                        if (!ok) {
                            finish(text);
                            return;
                        }
                        QVariantList messages;
                        for (const QVariantList &u : untagged) {
                            if (u.value(1).toByteArray().toUpper() != "FETCH")
                                continue;
                            const QVariantList items = u.value(2).toList();
                            QVariantMap m;
                            for (int i = 0; i + 1 < items.size(); i += 2) {
                                const QByteArray key = items.at(i).toByteArray().toUpper();
                                const QVariant val = items.at(i + 1);
                                if (key == "UID")
                                    m[QStringLiteral("uid")] = val.toByteArray().toUInt();
                                else if (key == "FLAGS")
                                    m[QStringLiteral("seen")] = [&] {
                                        for (const QVariant &f : val.toList())
                                            if (f.toByteArray().toLower() == "\\seen")
                                                return true;
                                        return false;
                                    }();
                                else if (key == "INTERNALDATE")
                                    m[QStringLiteral("received")] = QDateTime::fromString(
                                        QString::fromLatin1(val.toByteArray()), QStringLiteral("d-MMM-yyyy hh:mm:ss t"));
                                else if (key == "RFC822.SIZE")
                                    m[QStringLiteral("size")] = val.toByteArray().toInt();
                                else if (key == "ENVELOPE") {
                                    const QVariantList e = val.toList();
                                    m[QStringLiteral("date")] = QDateTime::fromString(
                                        QString::fromLatin1(e.value(0).toByteArray()).simplified(), Qt::RFC2822Date);
                                    m[QStringLiteral("subject")] = str(e.value(1));
                                    QString addr;
                                    m[QStringLiteral("from")] = addresses(e.value(2), &addr);
                                    m[QStringLiteral("fromAddress")] = addr;
                                    m[QStringLiteral("to")] = addresses(e.value(5));
                                    m[QStringLiteral("cc")] = addresses(e.value(6));
                                    m[QStringLiteral("messageId")] = QString::fromLatin1(e.value(9).toByteArray());
                                }
                            }
                            if (!m.value(QStringLiteral("date")).toDateTime().isValid())
                                m[QStringLiteral("date")] = m.value(QStringLiteral("received"));
                            messages.prepend(m);
                        }
                        // the newest first: by date, else by arrival
                        std::sort(messages.begin(), messages.end(), [](const QVariant &a, const QVariant &b) {
                            const QDateTime da = a.toMap().value(QStringLiteral("date")).toDateTime();
                            const QDateTime db = b.toMap().value(QStringLiteral("date")).toDateTime();
                            if (da.isValid() && db.isValid() && da != db)
                                return da > db;
                            return a.toMap().value(QStringLiteral("uid")).toUInt() > b.toMap().value(QStringLiteral("uid")).toUInt();
                        });
                        emit listed(folder, messages, folders);
                        finish();
                    });
                });
            });
        });
    });
}

void Imap::fetch(const Account &account, const QString &folder, uint uid)
{
    start(account, [this, folder, uid] {
        send("SELECT " + quote(folder), [this, folder, uid](bool ok, const QList<QVariantList> &, const QString &text) {
            if (!ok) {
                finish(text);
                return;
            }
            send("UID FETCH " + QByteArray::number(uid) + " (BODY.PEEK[])",
                 [this, folder, uid](bool ok, const QList<QVariantList> &untagged, const QString &text) {
                if (!ok) {
                    finish(text);
                    return;
                }
                QByteArray raw;
                for (const QVariantList &u : untagged) {
                    const QVariantList items = u.value(2).toList();
                    for (int i = 0; i + 1 < items.size(); i += 2)
                        if (items.at(i).toByteArray().toUpper().startsWith("BODY["))
                            raw = items.at(i + 1).toByteArray();
                }
                emit fetched(folder, uid, raw);
                // seen once read, as the N9's mail
                send("UID STORE " + QByteArray::number(uid) + " +FLAGS.SILENT (\\Seen)",
                     [this](bool, const QList<QVariantList> &, const QString &) { finish(); });
            });
        });
    });
}

void Imap::setSeen(const Account &account, const QString &folder, uint uid, bool seen)
{
    start(account, [this, folder, uid, seen] {
        send("SELECT " + quote(folder), [this, uid, seen](bool ok, const QList<QVariantList> &, const QString &text) {
            if (!ok) {
                finish(text);
                return;
            }
            send("UID STORE " + QByteArray::number(uid) + (seen ? " +" : " -") + "FLAGS.SILENT (\\Seen)",
                 [this](bool ok, const QList<QVariantList> &, const QString &text) { finish(ok ? QString() : text); });
        });
    });
}

void Imap::remove(const Account &account, const QString &folder, uint uid)
{
    start(account, [this, folder, uid] {
        send("SELECT " + quote(folder), [this, uid](bool ok, const QList<QVariantList> &, const QString &text) {
            if (!ok) {
                finish(text);
                return;
            }
            send("UID STORE " + QByteArray::number(uid) + " +FLAGS.SILENT (\\Deleted)",
                 [this](bool ok, const QList<QVariantList> &, const QString &text) {
                if (!ok) {
                    finish(text);
                    return;
                }
                send("EXPUNGE", [this](bool ok, const QList<QVariantList> &, const QString &text) {
                    finish(ok ? QString() : text);
                });
            });
        });
    });
}
