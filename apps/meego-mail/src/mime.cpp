#include "mime.h"

#include <QLocale>
#include <QRegularExpression>
#include <QTextCodec>
#include <QTextDocumentFragment>
#include <QUuid>

namespace Mime {

QString toUnicode(const QByteArray &bytes, const QByteArray &charset)
{
    QTextCodec *codec = charset.isEmpty() ? nullptr : QTextCodec::codecForName(charset.trimmed());
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");
    return codec->toUnicode(bytes);
}

QByteArray quotedPrintableDecode(const QByteArray &in, bool header)
{
    QByteArray out;
    out.reserve(in.size());
    for (int i = 0; i < in.size(); ++i) {
        const char c = in.at(i);
        if (c == '=' && i + 2 < in.size() + 0 && i + 1 < in.size()) {
            if (in.at(i + 1) == '\r' || in.at(i + 1) == '\n') {
                // a soft line break
                i += (in.at(i + 1) == '\r' && i + 2 < in.size() && in.at(i + 2) == '\n') ? 2 : 1;
                continue;
            }
            if (i + 2 < in.size()) {
                bool ok = false;
                const int v = in.mid(i + 1, 2).toInt(&ok, 16);
                if (ok) {
                    out += char(v);
                    i += 2;
                    continue;
                }
            }
        }
        out += (header && c == '_') ? ' ' : c;
    }
    return out;
}

// =?charset?B|Q?text?= words, the space between two of them dropped
QString decodeWords(const QByteArray &header)
{
    static const QRegularExpression word(QStringLiteral("=\\?([^?]+)\\?([BbQq])\\?([^?]*)\\?="));
    const QString s = QString::fromLatin1(header);
    QString out;
    int last = 0;
    bool afterWord = false;
    auto it = word.globalMatch(s);
    while (it.hasNext()) {
        const auto m = it.next();
        const QString between = s.mid(last, m.capturedStart() - last);
        if (!(afterWord && between.trimmed().isEmpty()))
            out += toUnicode(between.toLatin1(), "UTF-8");
        QByteArray charset = m.captured(1).toLatin1();
        const int star = charset.indexOf('*');     // RFC 2231 language
        if (star >= 0)
            charset.truncate(star);
        const QByteArray text = m.captured(3).toLatin1();
        const QByteArray bytes = m.captured(2).toUpper() == QLatin1String("B")
            ? QByteArray::fromBase64(text) : quotedPrintableDecode(text, true);
        out += toUnicode(bytes, charset);
        last = m.capturedEnd();
        afterWord = true;
    }
    out += toUnicode(s.mid(last).toLatin1(), "UTF-8");
    return out.simplified();
}

QByteArray encodeWord(const QString &text)
{
    for (const QChar c : text)
        if (c.unicode() > 126)
            return "=?UTF-8?B?" + text.toUtf8().toBase64() + "?=";
    return text.toLatin1();
}

struct Part {
    QMap<QByteArray, QByteArray> headers;    // lower-case names, unfolded
    QByteArray body;
};

static Part split(const QByteArray &raw)
{
    Part p;
    int end = raw.indexOf("\r\n\r\n");
    int skip = 4;
    const int lf = raw.indexOf("\n\n");
    if (end < 0 || (lf >= 0 && lf < end)) {
        end = lf;
        skip = 2;
    }
    const QByteArray head = end < 0 ? raw : raw.left(end);
    p.body = end < 0 ? QByteArray() : raw.mid(end + skip);
    QByteArray name, value;
    for (QByteArray line : head.split('\n')) {
        if (line.endsWith('\r'))
            line.chop(1);
        if (!line.isEmpty() && (line.at(0) == ' ' || line.at(0) == '\t')) {
            value += ' ' + line.trimmed();
            continue;
        }
        if (!name.isEmpty())
            p.headers.insert(name, value);
        const int colon = line.indexOf(':');
        if (colon <= 0) {
            name.clear();
            continue;
        }
        name = line.left(colon).trimmed().toLower();
        value = line.mid(colon + 1).trimmed();
    }
    if (!name.isEmpty())
        p.headers.insert(name, value);
    return p;
}

// a parameter of a header: Content-Type: text/plain; charset="utf-8"
static QByteArray param(const QByteArray &header, const QByteArray &name)
{
    const QRegularExpression re(QStringLiteral("(?:^|;)\\s*%1\\*?=\\s*(\"([^\"]*)\"|[^;\\s]+)")
                                .arg(QString::fromLatin1(name)), QRegularExpression::CaseInsensitiveOption);
    const auto m = re.match(QString::fromLatin1(header));
    if (!m.hasMatch())
        return QByteArray();
    QString v = m.captured(2).isEmpty() ? m.captured(1) : m.captured(2);
    // RFC 2231: utf-8''name%20with%20spaces
    const int q = v.indexOf(QLatin1String("''"));
    if (q > 0)
        v = QString::fromUtf8(QByteArray::fromPercentEncoding(v.mid(q + 2).toLatin1()));
    return v.toLatin1();
}

static QByteArray decodeBody(const Part &p)
{
    const QByteArray cte = p.headers.value("content-transfer-encoding").toLower();
    if (cte == "base64")
        return QByteArray::fromBase64(p.body);
    if (cte == "quoted-printable")
        return quotedPrintableDecode(p.body);
    return p.body;
}

// the best text of a part and its children; attachments' names on the way
static bool textOf(const Part &p, QString *text, bool *html, QStringList *attachments, int depth = 0)
{
    const QByteArray type = p.headers.value("content-type", "text/plain");
    const QByteArray mime = type.split(';').first().trimmed().toLower();
    const QByteArray disposition = p.headers.value("content-disposition").toLower();
    QByteArray filename = param(p.headers.value("content-disposition"), "filename");
    if (filename.isEmpty())
        filename = param(type, "name");

    if (mime.startsWith("multipart/") && depth < 10) {
        const QByteArray boundary = "--" + param(type, "boundary");
        if (boundary.size() <= 2)
            return false;
        QList<Part> parts;
        int pos = p.body.indexOf(boundary);
        while (pos >= 0) {
            pos += boundary.size();
            if (p.body.mid(pos, 2) == "--")
                break;
            const int next = p.body.indexOf(boundary, pos);
            QByteArray chunk = p.body.mid(pos, next < 0 ? -1 : next - pos);
            while (chunk.startsWith("\r\n") || chunk.startsWith("\n"))
                chunk = chunk.mid(chunk.startsWith("\r\n") ? 2 : 1);
            parts << split(chunk);
            pos = next;
        }
        bool found = false;
        if (mime == "multipart/alternative") {
            // plain text first, HTML when there is none
            for (const Part &c : parts)
                if (c.headers.value("content-type", "text/plain").toLower().startsWith("text/plain")
                    && textOf(c, text, html, attachments, depth + 1))
                    return true;
        }
        for (const Part &c : parts) {
            if (!found)
                found = textOf(c, text, html, attachments, depth + 1);
            else
                textOf(c, nullptr, nullptr, attachments, depth + 1);
        }
        return found;
    }
    if (disposition.startsWith("attachment") || (!filename.isEmpty() && !mime.startsWith("text/"))) {
        if (attachments)
            attachments->append(decodeWords(filename));
        return false;
    }
    if (!text || (mime != "text/plain" && mime != "text/html"))
        return false;
    const QString s = toUnicode(decodeBody(p), param(type, "charset"));
    if (mime == "text/html") {
        *text = QTextDocumentFragment::fromHtml(s).toPlainText();
        *html = true;
    } else {
        *text = s;
        *html = false;
    }
    return true;
}

Message parse(const QByteArray &raw)
{
    const Part p = split(raw);
    Message m;
    m.subject = decodeWords(p.headers.value("subject"));
    m.from = decodeWords(p.headers.value("from"));
    m.to = decodeWords(p.headers.value("to"));
    m.cc = decodeWords(p.headers.value("cc"));
    m.date = QDateTime::fromString(QString::fromLatin1(p.headers.value("date")).simplified(), Qt::RFC2822Date);
    textOf(p, &m.text, &m.html, &m.attachments);
    return m;
}

static QByteArray addressList(const QStringList &list)
{
    QByteArrayList out;
    for (const QString &a : list) {
        // "Name <x@y>": the name encoded when it has to be
        const int lt = a.indexOf(QLatin1Char('<'));
        if (lt > 0)
            out << encodeWord(a.left(lt).trimmed()) + " " + a.mid(lt).toUtf8();
        else
            out << a.trimmed().toUtf8();
    }
    return out.join(", ");
}

QByteArray compose(const QString &from, const QStringList &to, const QStringList &cc,
                   const QString &subject, const QString &body, const QString &inReplyTo)
{
    QByteArray m;
    const QString domain = from.section(QLatin1Char('@'), 1).remove(QLatin1Char('>'));
    m += "From: " + addressList({from}) + "\r\n";
    m += "To: " + addressList(to) + "\r\n";
    if (!cc.isEmpty())
        m += "Cc: " + addressList(cc) + "\r\n";
    m += "Subject: " + encodeWord(subject) + "\r\n";
    const QDateTime now = QDateTime::currentDateTime();
    const int off = now.offsetFromUtc() / 60;
    m += "Date: " + QLocale::c().toString(now, QStringLiteral("ddd, dd MMM yyyy hh:mm:ss ")).toLatin1()
         + QString::asprintf("%c%02d%02d", off < 0 ? '-' : '+', qAbs(off) / 60, qAbs(off) % 60).toLatin1()
         + "\r\n";
    m += "Message-ID: <" + QUuid::createUuid().toString(QUuid::WithoutBraces).toLatin1() + "@"
         + (domain.isEmpty() ? QByteArray("meego-mail") : domain.toLatin1()) + ">\r\n";
    if (!inReplyTo.isEmpty())
        m += "In-Reply-To: " + inReplyTo.toLatin1() + "\r\nReferences: " + inReplyTo.toLatin1() + "\r\n";
    m += "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=UTF-8\r\n"
         "Content-Transfer-Encoding: base64\r\nUser-Agent: meego-mail\r\n\r\n";
    const QByteArray b64 = body.toUtf8().toBase64();
    for (int i = 0; i < b64.size(); i += 76)
        m += b64.mid(i, 76) + "\r\n";
    return m;
}

}
