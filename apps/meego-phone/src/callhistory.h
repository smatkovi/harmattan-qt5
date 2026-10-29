// The call history, newest first, kept by the app itself (a JSON file).
#ifndef CALLHISTORY_H
#define CALLHISTORY_H

#include <QDateTime>
#include <QObject>
#include <QVariantList>

class CallHistory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList calls READ calls NOTIFY changed)
    Q_PROPERTY(int missed READ missed NOTIFY changed)

public:
    explicit CallHistory(QObject *parent = nullptr);

    // each: number, kind ("received", "initiated", "missed"), start, seconds
    QVariantList calls() const { return m_calls; }
    int missed() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(int index);

public slots:
    void add(const QString &number, bool incoming, bool answered, const QDateTime &start, int seconds);

signals:
    void changed();

private:
    void save() const;

    QString m_file;
    QVariantList m_calls;
};

#endif
