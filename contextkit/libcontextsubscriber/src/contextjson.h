/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef CONTEXTJSON_H
#define CONTEXTJSON_H

#include <climits>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

// stolen from glib
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define CKIT_GNUC_UNUSED __attribute__((__unused__))
#else
#define CKIT_GNUC_UNUSED
#endif

namespace ContextSubscriber {

// Please note that it is intentional that this header file contains
// implementation.  With Qt 5 the JSON support is in QtCore, so nothing
// extra has to be linked.

// QJsonDocument only holds an object or an array, so a single value
// travels as the one element of an array.  JSON numbers become integers
// where they are whole, as QJson made them.
CKIT_GNUC_UNUSED
static QVariant jsonValueToQVariant(const QJsonValue &v) {
    switch (v.type()) {
    case QJsonValue::Double: {
        double d = v.toDouble();
        if (d == (double) (qlonglong) d)
            return (d >= INT_MIN && d <= INT_MAX)
                ? QVariant((int) d) : QVariant((qlonglong) d);
        return QVariant(d);
    }
    case QJsonValue::Array: {
        QVariantList l;
        foreach (const QJsonValue &e, v.toArray())
            l << jsonValueToQVariant(e);
        return l;
    }
    case QJsonValue::Object: {
        QVariantMap m;
        QJsonObject o = v.toObject();
        for (QJsonObject::const_iterator i = o.constBegin(); i != o.constEnd(); ++i)
            m.insert(i.key(), jsonValueToQVariant(i.value()));
        return m;
    }
    case QJsonValue::Null:
    case QJsonValue::Undefined:
        // an invalid QVariant, as QJson gave (not one holding nullptr,
        // D-Bus can't carry that)
        return QVariant();
    default:
        return v.toVariant();
    }
}

/// Dumps a QVariant into a QString in JSON format.  This is very
/// useful if the value is quite complex (list of maps of lists) and
/// you would like to see it for debugging reasons.
CKIT_GNUC_UNUSED
static QString qVariantToJSON(const QVariant &v) {
    QJsonArray a;
    a.append(QJsonValue::fromVariant(v));
    QByteArray json = QJsonDocument(a).toJson(QJsonDocument::Compact);
    // without the brackets of the array
    return QString::fromUtf8(json.mid(1, json.size() - 2));
}

/// Loads json data into a QVariant.  On parse error it returns QVariant()
/// and sets *ok (when given) to false.
CKIT_GNUC_UNUSED
static QVariant jsonToQVariant(const QString &s, bool *ok = 0) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson("[" + s.toUtf8() + "]", &error);
    bool parsed = error.error == QJsonParseError::NoError
        && doc.array().size() == 1;
    if (ok)
        *ok = parsed;
    if (!parsed)
        return QVariant();
    return jsonValueToQVariant(doc.array().at(0));
}

} // namespace

#endif
