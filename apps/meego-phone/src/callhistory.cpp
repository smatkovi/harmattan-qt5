#include "callhistory.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>

static const int keep = 500;

CallHistory::CallHistory(QObject *parent) : QObject(parent)
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    m_file = dir + QStringLiteral("/calls.json");
    QFile f(m_file);
    if (f.open(QIODevice::ReadOnly)) {
        for (const QVariant &v : QJsonDocument::fromJson(f.readAll()).array().toVariantList()) {
            QVariantMap m = v.toMap();
            m[QStringLiteral("start")] = QDateTime::fromString(m.value(QStringLiteral("start")).toString(), Qt::ISODate);
            m_calls << m;
        }
    }
}

int CallHistory::missed() const
{
    int n = 0;
    for (const QVariant &v : m_calls) {
        if (v.toMap().value(QStringLiteral("kind")) != QLatin1String("missed"))
            break;
        ++n;
    }
    return n;
}

void CallHistory::add(const QString &number, bool incoming, bool answered, const QDateTime &start, int seconds)
{
    const QString kind = !incoming ? QStringLiteral("initiated")
                       : answered ? QStringLiteral("received") : QStringLiteral("missed");
    m_calls.prepend(QVariantMap{{QStringLiteral("number"), number}, {QStringLiteral("kind"), kind},
                                {QStringLiteral("start"), start}, {QStringLiteral("seconds"), seconds}});
    while (m_calls.size() > keep)
        m_calls.removeLast();
    save();
    emit changed();
}

void CallHistory::clear()
{
    m_calls.clear();
    save();
    emit changed();
}

void CallHistory::remove(int index)
{
    if (index >= 0 && index < m_calls.size()) {
        m_calls.removeAt(index);
        save();
        emit changed();
    }
}

void CallHistory::save() const
{
    QJsonArray a;
    for (const QVariant &v : m_calls) {
        QVariantMap m = v.toMap();
        m[QStringLiteral("start")] = m.value(QStringLiteral("start")).toDateTime().toString(Qt::ISODate);
        a.append(QJsonValue::fromVariant(m));
    }
    QFile f(m_file);
    if (f.open(QIODevice::WriteOnly))
        f.write(QJsonDocument(a).toJson(QJsonDocument::Compact));
}
