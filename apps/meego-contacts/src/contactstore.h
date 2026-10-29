// The contacts: one vCard per contact in the app's folder, the photos as
// files for the QML; calls, messages and mails through the other apps.
#ifndef CONTACTSTORE_H
#define CONTACTSTORE_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class ContactStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY changed)
    Q_PROPERTY(QString folder READ folder CONSTANT)

public:
    explicit ContactStore(QObject *parent = nullptr);

    int count() const { return m_cards.size(); }
    QString folder() const { return m_dir; }

    // sorted by name, each with its section (first letter) and photo URL;
    // filter: in the name, an organisation, a number or an address
    Q_INVOKABLE QVariantList contacts(const QString &filter = QString()) const;
    Q_INVOKABLE QVariantMap contact(const QString &uid) const;
    Q_INVOKABLE QString save(const QVariantMap &contact);
    Q_INVOKABLE void remove(const QString &uid);
    // the .vcf files in Documents and Downloads; returns how many came
    Q_INVOKABLE int importFiles();
    // "call", "sms" or "email": the phone, messages or mail app
    Q_INVOKABLE void launch(const QString &what, const QString &value);

signals:
    void changed();

private:
    void load();
    QVariantMap forQml(const QVariantMap &card) const;

    QString m_dir, m_photoDir;
    QList<QVariantMap> m_cards;
};

#endif
