// What mail needs of MIME: header words in their charsets (RFC 2047), the
// transfer encodings, the text of a message (text/plain, else text/html
// made plain), its attachments' names; and a plain text message to send.
#ifndef MIME_H
#define MIME_H

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QStringList>

namespace Mime {

QString decodeWords(const QByteArray &header);
QByteArray encodeWord(const QString &text);
QByteArray quotedPrintableDecode(const QByteArray &in, bool header = false);
QString toUnicode(const QByteArray &bytes, const QByteArray &charset);

struct Message {
    QString subject, from, to, cc, text;
    QDateTime date;
    bool html = false;          // text came from text/html
    QStringList attachments;    // their file names
};

Message parse(const QByteArray &raw);

QByteArray compose(const QString &from, const QStringList &to, const QStringList &cc,
                   const QString &subject, const QString &body, const QString &inReplyTo = QString());

}

#endif
