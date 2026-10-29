// The calendar's events in a SQLite database of their own, with their
// repetitions unfolded for the days asked for.
#ifndef EVENTSTORE_H
#define EVENTSTORE_H

#include <QDate>
#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>
#include <QVariantMap>

class EventStore : public QObject
{
    Q_OBJECT

public:
    explicit EventStore(QObject *parent = nullptr);

    // the occurrences on one day, all-day ones first, then by time; each
    // a map of the event's fields with this occurrence's start and end
    Q_INVOKABLE QVariantList eventsOn(const QDate &day) const;
    // the days of a month (1-31) that have events
    Q_INVOKABLE QVariantList busyDays(int year, int month) const;
    Q_INVOKABLE QVariantMap event(int id) const;
    // a new event (id 0) or a changed one; returns its id
    Q_INVOKABLE int save(const QVariantMap &event);
    Q_INVOKABLE void remove(int id);
    // the occurrences from today on, for the agenda (at most days ahead)
    Q_INVOKABLE QVariantList upcoming(int days) const;

signals:
    void changed();

private:
    struct Event {
        int id = 0;
        QString title, location, notes, recur;
        QDateTime start, end, until;
        bool allDay = false;
        int alert = -1;
        int color = 0;
    };

    QList<Event> all() const;
    static bool occursOn(const Event &e, const QDate &day, QDateTime *start, QDateTime *end);
    static QVariantMap toMap(const Event &e, const QDateTime &start, const QDateTime &end);

    QSqlDatabase m_db;
};

#endif
