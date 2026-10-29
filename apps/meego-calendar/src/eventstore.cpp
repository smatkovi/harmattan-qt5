#include "eventstore.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDebug>
#include <algorithm>

EventStore::EventStore(QObject *parent) : QObject(parent)
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("events"));
    m_db.setDatabaseName(dir + QStringLiteral("/events.sqlite"));
    if (!m_db.open()) {
        qWarning() << "meego-calendar: no database:" << m_db.lastError().text();
        return;
    }
    QSqlQuery q(m_db);
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS events ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, location TEXT, notes TEXT,"
        " start INTEGER, end INTEGER, allday INTEGER, recur TEXT, until INTEGER,"
        " alert INTEGER, color INTEGER)"));
}

QList<EventStore::Event> EventStore::all() const
{
    QList<Event> list;
    QSqlQuery q(m_db);
    q.exec(QStringLiteral("SELECT id, title, location, notes, start, end, allday, recur, until,"
                          " alert, color FROM events"));
    while (q.next()) {
        Event e;
        e.id = q.value(0).toInt();
        e.title = q.value(1).toString();
        e.location = q.value(2).toString();
        e.notes = q.value(3).toString();
        e.start = QDateTime::fromSecsSinceEpoch(q.value(4).toLongLong());
        e.end = QDateTime::fromSecsSinceEpoch(q.value(5).toLongLong());
        e.allDay = q.value(6).toBool();
        e.recur = q.value(7).toString();
        if (q.value(8).toLongLong() > 0)
            e.until = QDateTime::fromSecsSinceEpoch(q.value(8).toLongLong());
        e.alert = q.value(9).toInt();
        e.color = q.value(10).toInt();
        list << e;
    }
    return list;
}

// Whether an event (or one of its repetitions) touches a day, and the
// start and end of that occurrence. Repetitions keep the event's length.
bool EventStore::occursOn(const Event &e, const QDate &day, QDateTime *start, QDateTime *end)
{
    const QDate first = e.start.date();
    const qint64 length = e.start.secsTo(e.end);
    // the days an occurrence lasts, beyond its first
    const int extra = qMax(0, int(first.daysTo(e.end.addSecs(e.allDay ? -1 : 0).date())));
    if (day < first || (e.until.isValid() && day > e.until.date().addDays(extra)))
        return false;

    // the occurrence that would start on 'd'
    auto startsOn = [&](const QDate &d) -> bool {
        if (d < first || (e.until.isValid() && d > e.until.date()))
            return false;
        if (e.recur == QLatin1String("daily"))
            return true;
        if (e.recur == QLatin1String("weekly"))
            return d.dayOfWeek() == first.dayOfWeek();
        if (e.recur == QLatin1String("monthly"))
            return d.day() == first.day();
        if (e.recur == QLatin1String("yearly"))
            return d.day() == first.day() && d.month() == first.month();
        return d == first;
    };
    for (int back = 0; back <= extra; ++back) {
        const QDate d = day.addDays(-back);
        if (startsOn(d)) {
            *start = QDateTime(d, e.start.time());
            *end = start->addSecs(length);
            return true;
        }
    }
    return false;
}

QVariantMap EventStore::toMap(const Event &e, const QDateTime &start, const QDateTime &end)
{
    return QVariantMap{
        {QStringLiteral("id"), e.id},
        {QStringLiteral("title"), e.title},
        {QStringLiteral("location"), e.location},
        {QStringLiteral("notes"), e.notes},
        {QStringLiteral("start"), start},
        {QStringLiteral("end"), end},
        {QStringLiteral("allDay"), e.allDay},
        {QStringLiteral("recur"), e.recur.isEmpty() ? QStringLiteral("none") : e.recur},
        {QStringLiteral("until"), e.until.isValid() ? QVariant(e.until.date()) : QVariant()},
        {QStringLiteral("alert"), e.alert},
        {QStringLiteral("color"), e.color},
    };
}

QVariantList EventStore::eventsOn(const QDate &day) const
{
    struct Occ { Event e; QDateTime s, t; };
    QList<Occ> occ;
    for (const Event &e : all()) {
        QDateTime s, t;
        if (occursOn(e, day, &s, &t))
            occ << Occ{e, s, t};
    }
    std::sort(occ.begin(), occ.end(), [](const Occ &a, const Occ &b) {
        if (a.e.allDay != b.e.allDay)
            return a.e.allDay;
        return a.s < b.s;
    });
    QVariantList list;
    for (const Occ &o : occ)
        list << toMap(o.e, o.s, o.t);
    return list;
}

QVariantList EventStore::busyDays(int year, int month) const
{
    const QList<Event> events = all();
    QVariantList days;
    const QDate first(year, month, 1);
    for (int d = 0; d < first.daysInMonth(); ++d) {
        const QDate day = first.addDays(d);
        QDateTime s, t;
        for (const Event &e : events) {
            if (occursOn(e, day, &s, &t)) {
                days << day.day();
                break;
            }
        }
    }
    return days;
}

QVariantList EventStore::upcoming(int days) const
{
    QVariantList list;
    const QDate today = QDate::currentDate();
    for (int d = 0; d < days; ++d) {
        const QVariantList on = eventsOn(today.addDays(d));
        for (QVariant v : on) {
            QVariantMap m = v.toMap();
            m.insert(QStringLiteral("day"), today.addDays(d));
            list << m;
        }
    }
    return list;
}

QVariantMap EventStore::event(int id) const
{
    for (const Event &e : all())
        if (e.id == id)
            return toMap(e, e.start, e.end);
    return QVariantMap();
}

int EventStore::save(const QVariantMap &m)
{
    const int id = m.value(QStringLiteral("id")).toInt();
    QSqlQuery q(m_db);
    if (id > 0)
        q.prepare(QStringLiteral("UPDATE events SET title=?, location=?, notes=?, start=?, end=?,"
                                 " allday=?, recur=?, until=?, alert=?, color=? WHERE id=?"));
    else
        q.prepare(QStringLiteral("INSERT INTO events (title, location, notes, start, end, allday,"
                                 " recur, until, alert, color) VALUES (?,?,?,?,?,?,?,?,?,?)"));
    QDateTime start = m.value(QStringLiteral("start")).toDateTime();
    QDateTime end = m.value(QStringLiteral("end")).toDateTime();
    const bool allDay = m.value(QStringLiteral("allDay")).toBool();
    if (allDay) {
        // whole days: midnight to the midnight after the last day
        start = QDateTime(start.date(), QTime(0, 0));
        end = QDateTime(qMax(end.date(), start.date()).addDays(1), QTime(0, 0));
    } else if (end < start) {
        end = start.addSecs(3600);
    }
    const QDate until = m.value(QStringLiteral("until")).toDate();
    QString recur = m.value(QStringLiteral("recur")).toString();
    if (recur == QLatin1String("none"))
        recur.clear();
    q.addBindValue(m.value(QStringLiteral("title")).toString());
    q.addBindValue(m.value(QStringLiteral("location")).toString());
    q.addBindValue(m.value(QStringLiteral("notes")).toString());
    q.addBindValue(start.toSecsSinceEpoch());
    q.addBindValue(end.toSecsSinceEpoch());
    q.addBindValue(allDay ? 1 : 0);
    q.addBindValue(recur);
    q.addBindValue(until.isValid() ? QDateTime(until, QTime(23, 59, 59)).toSecsSinceEpoch() : 0);
    q.addBindValue(m.value(QStringLiteral("alert"), -1).toInt());
    q.addBindValue(m.value(QStringLiteral("color")).toInt());
    if (id > 0)
        q.addBindValue(id);
    if (!q.exec()) {
        qWarning() << "meego-calendar: not saved:" << q.lastError().text();
        return 0;
    }
    emit changed();
    return id > 0 ? id : q.lastInsertId().toInt();
}

void EventStore::remove(int id)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("DELETE FROM events WHERE id=?"));
    q.addBindValue(id);
    q.exec();
    emit changed();
}
