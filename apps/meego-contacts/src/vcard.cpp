#include "vcard.h"

#include <QStringList>
#include <QTextCodec>
#include <QUuid>

namespace VCard {

static QString unescape(const QString &s)
{
    QString out;
    for (int i = 0; i < s.size(); ++i) {
        if (s.at(i) == QLatin1Char('\\') && i + 1 < s.size()) {
            const QChar n = s.at(++i);
            out += (n == QLatin1Char('n') || n == QLatin1Char('N')) ? QChar(QLatin1Char('\n')) : n;
        } else {
            out += s.at(i);
        }
    }
    return out;
}

static QString escape(const QString &s)
{
    QString out = s;
    out.replace(QLatin1String("\\"), QLatin1String("\\\\")).replace(QLatin1String(","), QLatin1String("\\,"))
       .replace(QLatin1String(";"), QLatin1String("\\;")).replace(QLatin1String("\n"), QLatin1String("\\n"));
    return out;
}

// split on a separator that is not escaped
static QStringList splitUnescaped(const QString &s, QChar sep)
{
    QStringList parts;
    QString cur;
    for (int i = 0; i < s.size(); ++i) {
        if (s.at(i) == QLatin1Char('\\') && i + 1 < s.size()) {
            cur += s.at(i);
            cur += s.at(++i);
        } else if (s.at(i) == sep) {
            parts << cur;
            cur.clear();
        } else {
            cur += s.at(i);
        }
    }
    parts << cur;
    return parts;
}

static QByteArray qpDecode(const QByteArray &in)
{
    QByteArray out;
    for (int i = 0; i < in.size(); ++i) {
        if (in.at(i) == '=' && i + 2 < in.size() + 1 && i + 2 <= in.size() - 1) {
            bool ok = false;
            const int v = in.mid(i + 1, 2).toInt(&ok, 16);
            if (ok) {
                out += char(v);
                i += 2;
                continue;
            }
        }
        out += in.at(i);
    }
    return out;
}

QList<QVariantMap> parse(const QByteArray &data)
{
    // unfold: a line starting with a space or tab continues the one
    // before; vCard 2.1's quoted-printable soft breaks too
    QList<QByteArray> lines;
    for (QByteArray l : data.split('\n')) {
        if (l.endsWith('\r'))
            l.chop(1);
        if (!lines.isEmpty() && !l.isEmpty() && (l.at(0) == ' ' || l.at(0) == '\t'))
            lines.last() += l.mid(1);
        else if (!lines.isEmpty() && lines.last().endsWith('=') && lines.last().toUpper().contains("QUOTED-PRINTABLE"))
            lines.last() = lines.last().left(lines.last().size() - 1) + l;
        else
            lines << l;
    }

    QList<QVariantMap> cards;
    QVariantMap c;
    QVariantList phones, emails;
    for (const QByteArray &line : lines) {
        const int colon = line.indexOf(':');
        if (colon <= 0)
            continue;
        const QByteArray head = line.left(colon);
        QByteArray value = line.mid(colon + 1);
        QList<QByteArray> params = head.split(';');
        QByteArray name = params.takeFirst().toUpper();
        const int dot = name.indexOf('.');     // item1.TEL
        if (dot >= 0)
            name = name.mid(dot + 1);
        QByteArray charset = "UTF-8";
        QStringList types;
        bool qp = false, b64 = false;
        for (const QByteArray &p : params) {
            const QByteArray up = p.toUpper();
            if (up == "ENCODING=QUOTED-PRINTABLE" || up == "QUOTED-PRINTABLE")
                qp = true;
            else if (up == "ENCODING=B" || up == "ENCODING=BASE64" || up == "BASE64")
                b64 = true;
            else if (up.startsWith("CHARSET="))
                charset = p.mid(8);
            else if (up.startsWith("TYPE="))
                for (const QByteArray &t : p.mid(5).split(','))
                    types << QString::fromLatin1(t).toLower().remove(QLatin1Char('"'));
            else if (!up.contains('='))
                types << QString::fromLatin1(p).toLower();
        }
        if (qp)
            value = qpDecode(value);
        QTextCodec *codec = QTextCodec::codecForName(charset);
        const QString text = codec ? codec->toUnicode(value) : QString::fromUtf8(value);

        if (name == "BEGIN") {
            c.clear();
            phones.clear();
            emails.clear();
        } else if (name == "END") {
            c[QStringLiteral("phones")] = phones;
            c[QStringLiteral("emails")] = emails;
            // without a UID one from what the card says: the same card
            // imported again is the same contact
            if (c.value(QStringLiteral("uid")).toString().isEmpty()) {
                QByteArray key = c.value(QStringLiteral("name")).toString().toUtf8() + '|'
                                 + c.value(QStringLiteral("first")).toString().toUtf8() + '|'
                                 + c.value(QStringLiteral("last")).toString().toUtf8();
                for (const QVariant &p : phones)
                    key += '|' + p.toMap().value(QStringLiteral("value")).toString().toUtf8();
                for (const QVariant &e : emails)
                    key += '|' + e.toMap().value(QStringLiteral("value")).toString().toUtf8();
                c[QStringLiteral("uid")] = QUuid::createUuidV5(QUuid(), key).toString(QUuid::WithoutBraces);
            }
            if (c.value(QStringLiteral("name")).toString().isEmpty())
                c[QStringLiteral("name")] = (c.value(QStringLiteral("first")).toString() + QLatin1Char(' ')
                                             + c.value(QStringLiteral("last")).toString()).trimmed();
            cards << c;
        } else if (name == "UID") {
            c[QStringLiteral("uid")] = text.trimmed();
        } else if (name == "FN") {
            c[QStringLiteral("name")] = unescape(text);
        } else if (name == "N") {
            const QStringList n = splitUnescaped(text, QLatin1Char(';'));
            c[QStringLiteral("last")] = unescape(n.value(0));
            c[QStringLiteral("first")] = unescape(n.value(1));
        } else if (name == "ORG") {
            c[QStringLiteral("org")] = unescape(splitUnescaped(text, QLatin1Char(';')).value(0));
        } else if (name == "TEL") {
            QString type = QStringLiteral("phone");
            if (types.contains(QLatin1String("cell")) || types.contains(QLatin1String("mobile")))
                type = QStringLiteral("mobile");
            else if (types.contains(QLatin1String("work")))
                type = QStringLiteral("work");
            else if (types.contains(QLatin1String("home")))
                type = QStringLiteral("home");
            phones << QVariantMap{{QStringLiteral("type"), type},
                                  {QStringLiteral("value"), QString(text).remove(QLatin1String("tel:"))}};
        } else if (name == "EMAIL") {
            emails << QVariantMap{{QStringLiteral("type"), types.contains(QLatin1String("work")) ? QStringLiteral("work") : QStringLiteral("home")},
                                  {QStringLiteral("value"), text.trimmed()}};
        } else if (name == "ADR") {
            QStringList a;
            for (const QString &p : splitUnescaped(text, QLatin1Char(';')))
                if (!p.trimmed().isEmpty())
                    a << unescape(p).trimmed();
            c[QStringLiteral("address")] = a.join(QStringLiteral(", "));
        } else if (name == "BDAY") {
            QString b = text.trimmed();
            if (b.size() == 8 && !b.contains(QLatin1Char('-')))
                b = b.left(4) + QLatin1Char('-') + b.mid(4, 2) + QLatin1Char('-') + b.mid(6, 2);
            c[QStringLiteral("birthday")] = b.left(10);
        } else if (name == "NOTE") {
            c[QStringLiteral("note")] = unescape(text);
        } else if (name == "PHOTO") {
            if (b64 || !value.startsWith("data:"))
                c[QStringLiteral("photo")] = QByteArray::fromBase64(value);
            else
                c[QStringLiteral("photo")] = QByteArray::fromBase64(value.mid(value.indexOf(',') + 1));
        }
    }
    return cards;
}

QByteArray write(const QVariantMap &c)
{
    QStringList out;
    out << QStringLiteral("BEGIN:VCARD") << QStringLiteral("VERSION:3.0");
    out << QStringLiteral("UID:") + c.value(QStringLiteral("uid")).toString();
    out << QStringLiteral("N:") + escape(c.value(QStringLiteral("last")).toString()) + QLatin1Char(';')
           + escape(c.value(QStringLiteral("first")).toString()) + QStringLiteral(";;;");
    out << QStringLiteral("FN:") + escape(c.value(QStringLiteral("name")).toString());
    if (!c.value(QStringLiteral("org")).toString().isEmpty())
        out << QStringLiteral("ORG:") + escape(c.value(QStringLiteral("org")).toString());
    for (const QVariant &v : c.value(QStringLiteral("phones")).toList()) {
        const QVariantMap p = v.toMap();
        if (p.value(QStringLiteral("value")).toString().trimmed().isEmpty())
            continue;
        const QString t = p.value(QStringLiteral("type")).toString();
        const QString type = t == QLatin1String("mobile") ? QStringLiteral("CELL")
                           : t == QLatin1String("work") ? QStringLiteral("WORK")
                           : t == QLatin1String("home") ? QStringLiteral("HOME") : QStringLiteral("VOICE");
        out << QStringLiteral("TEL;TYPE=") + type + QLatin1Char(':') + p.value(QStringLiteral("value")).toString().trimmed();
    }
    for (const QVariant &v : c.value(QStringLiteral("emails")).toList()) {
        const QVariantMap e = v.toMap();
        if (e.value(QStringLiteral("value")).toString().trimmed().isEmpty())
            continue;
        out << QStringLiteral("EMAIL;TYPE=") + (e.value(QStringLiteral("type")).toString() == QLatin1String("work")
                                                 ? QStringLiteral("WORK") : QStringLiteral("HOME"))
               + QLatin1Char(':') + e.value(QStringLiteral("value")).toString().trimmed();
    }
    if (!c.value(QStringLiteral("address")).toString().isEmpty())
        out << QStringLiteral("ADR:;;") + escape(c.value(QStringLiteral("address")).toString()) + QStringLiteral(";;;;");
    if (!c.value(QStringLiteral("birthday")).toString().isEmpty())
        out << QStringLiteral("BDAY:") + c.value(QStringLiteral("birthday")).toString();
    if (!c.value(QStringLiteral("note")).toString().isEmpty())
        out << QStringLiteral("NOTE:") + escape(c.value(QStringLiteral("note")).toString());
    const QByteArray photo = c.value(QStringLiteral("photo")).toByteArray();
    if (!photo.isEmpty())
        out << QStringLiteral("PHOTO;ENCODING=b;TYPE=JPEG:") + QString::fromLatin1(photo.toBase64());
    out << QStringLiteral("END:VCARD");
    // lines of at most 75 octets, folded
    QByteArray b;
    for (const QString &l : out) {
        QByteArray u = l.toUtf8();
        while (u.size() > 75) {
            int cut = 75;
            while (cut > 0 && (uchar(u.at(cut)) & 0xC0) == 0x80)   // not inside a UTF-8 character
                --cut;
            b += u.left(cut) + "\r\n ";
            u = u.mid(cut);
        }
        b += u + "\r\n";
    }
    return b;
}

}
