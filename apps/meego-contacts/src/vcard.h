// vCard 2.1, 3.0 and 4.0 in, 3.0 out: the fields a phone's contacts have.
#ifndef VCARD_H
#define VCARD_H

#include <QByteArray>
#include <QList>
#include <QVariantMap>

namespace VCard {

// each card: uid, first, last, name (formatted), org, phones [{type, value}],
// emails [{type, value}], address, birthday (yyyy-mm-dd), note, photo
// (bytes, decoded)
QList<QVariantMap> parse(const QByteArray &data);
QByteArray write(const QVariantMap &card);

}

#endif
