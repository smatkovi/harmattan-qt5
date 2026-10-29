#include "contactstore.h"
#include "vcard.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>
#include <QUuid>
#include <algorithm>

ContactStore::ContactStore(QObject *parent) : QObject(parent)
{
    m_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/contacts");
    m_photoDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/photos");
    QDir().mkpath(m_dir);
    QDir().mkpath(m_photoDir);
    load();
}

void ContactStore::load()
{
    m_cards.clear();
    for (const QString &f : QDir(m_dir).entryList({QStringLiteral("*.vcf")}, QDir::Files)) {
        QFile file(m_dir + QLatin1Char('/') + f);
        if (file.open(QIODevice::ReadOnly))
            m_cards << VCard::parse(file.readAll());
    }
    std::sort(m_cards.begin(), m_cards.end(), [](const QVariantMap &a, const QVariantMap &b) {
        return QString::localeAwareCompare(a.value(QStringLiteral("name")).toString(),
                                           b.value(QStringLiteral("name")).toString()) < 0;
    });
    emit changed();
}

QVariantMap ContactStore::forQml(const QVariantMap &card) const
{
    QVariantMap m = card;
    const QByteArray photo = m.take(QStringLiteral("photo")).toByteArray();
    QString url;
    if (!photo.isEmpty()) {
        const QString path = m_photoDir + QLatin1Char('/') + card.value(QStringLiteral("uid")).toString() + QStringLiteral(".jpg");
        QFile f(path);
        if (f.size() != photo.size() && f.open(QIODevice::WriteOnly))
            f.write(photo);
        url = QUrl::fromLocalFile(path).toString();
    }
    m[QStringLiteral("photoUrl")] = url;
    const QString name = m.value(QStringLiteral("name")).toString();
    const QChar first = name.isEmpty() ? QLatin1Char('#') : name.at(0).toUpper();
    m[QStringLiteral("section")] = first.isLetter() ? QString(first) : QStringLiteral("#");
    return m;
}

QVariantList ContactStore::contacts(const QString &filter) const
{
    QVariantList list;
    const QString f = filter.trimmed();
    for (const QVariantMap &c : m_cards) {
        if (!f.isEmpty()) {
            QString hay = c.value(QStringLiteral("name")).toString() + QLatin1Char(' ')
                          + c.value(QStringLiteral("org")).toString() + QLatin1Char(' ')
                          + c.value(QStringLiteral("address")).toString();
            for (const QVariant &p : c.value(QStringLiteral("phones")).toList())
                hay += QLatin1Char(' ') + p.toMap().value(QStringLiteral("value")).toString().remove(QLatin1Char(' '));
            for (const QVariant &e : c.value(QStringLiteral("emails")).toList())
                hay += QLatin1Char(' ') + e.toMap().value(QStringLiteral("value")).toString();
            if (!hay.contains(f, Qt::CaseInsensitive))
                continue;
        }
        list << forQml(c);
    }
    return list;
}

QVariantMap ContactStore::contact(const QString &uid) const
{
    for (const QVariantMap &c : m_cards)
        if (c.value(QStringLiteral("uid")).toString() == uid)
            return forQml(c);
    return QVariantMap();
}

QString ContactStore::save(const QVariantMap &contact)
{
    QVariantMap c = contact;
    QString uid = c.value(QStringLiteral("uid")).toString();
    if (uid.isEmpty())
        uid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    c[QStringLiteral("uid")] = uid;
    const QString name = (c.value(QStringLiteral("first")).toString() + QLatin1Char(' ')
                          + c.value(QStringLiteral("last")).toString()).trimmed();
    c[QStringLiteral("name")] = name.isEmpty() ? c.value(QStringLiteral("org")).toString() : name;
    // the photo stays as it was
    for (const QVariantMap &old : m_cards)
        if (old.value(QStringLiteral("uid")).toString() == uid)
            c[QStringLiteral("photo")] = old.value(QStringLiteral("photo"));
    QFile f(m_dir + QLatin1Char('/') + uid + QStringLiteral(".vcf"));
    if (f.open(QIODevice::WriteOnly))
        f.write(VCard::write(c));
    load();
    return uid;
}

void ContactStore::remove(const QString &uid)
{
    QFile::remove(m_dir + QLatin1Char('/') + uid + QStringLiteral(".vcf"));
    QFile::remove(m_photoDir + QLatin1Char('/') + uid + QStringLiteral(".jpg"));
    load();
}

int ContactStore::importFiles()
{
    int n = 0;
    QStringList dirs;
    for (const QString &d : {QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                             QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)}) {
        const QString canonical = QDir(d).canonicalPath();
        if (!canonical.isEmpty() && !dirs.contains(canonical))
            dirs << canonical;
    }
    for (const QString &d : dirs) {
        for (const QString &f : QDir(d).entryList({QStringLiteral("*.vcf"), QStringLiteral("*.VCF")}, QDir::Files)) {
            QFile file(d + QLatin1Char('/') + f);
            if (!file.open(QIODevice::ReadOnly))
                continue;
            for (QVariantMap card : VCard::parse(file.readAll())) {
                // one file per contact, under its own uid
                QFile out(m_dir + QLatin1Char('/') + card.value(QStringLiteral("uid")).toString() + QStringLiteral(".vcf"));
                if (out.exists())
                    continue;
                if (out.open(QIODevice::WriteOnly)) {
                    out.write(VCard::write(card));
                    ++n;
                }
            }
        }
    }
    load();
    return n;
}

void ContactStore::launch(const QString &what, const QString &value)
{
    QString app, url;
    if (what == QLatin1String("call")) {
        app = QStringLiteral("meego-phone");
        url = QStringLiteral("tel:") + value;
    } else if (what == QLatin1String("sms")) {
        app = QStringLiteral("meego-messages");
        url = QStringLiteral("sms:") + value;
    } else {
        app = QStringLiteral("meego-mail");
        url = QStringLiteral("mailto:") + value;
    }
    if (QStandardPaths::findExecutable(app).isEmpty() || !QProcess::startDetached(app, {url}))
        QDesktopServices::openUrl(QUrl(url));
}
