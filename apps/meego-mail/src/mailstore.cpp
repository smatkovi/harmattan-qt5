#include "mailstore.h"
#include "mime.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>

static const int listSize = 60;

MailStore::MailStore(QObject *parent) : QObject(parent)
{
    m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(m_cacheDir + QStringLiteral("/bodies"));

    QSettings s(QStringLiteral("meego"), QStringLiteral("meego-mail"));
    m_account.name = s.value(QStringLiteral("name")).toString();
    m_account.address = s.value(QStringLiteral("address")).toString();
    m_account.user = s.value(QStringLiteral("user")).toString();
    m_account.password = s.value(QStringLiteral("password")).toString();
    m_account.imapHost = s.value(QStringLiteral("imapHost")).toString();
    m_account.imapPort = s.value(QStringLiteral("imapPort"), 993).toInt();
    m_account.imapSecurity = s.value(QStringLiteral("imapSecurity"), QStringLiteral("ssl")).toString();
    m_account.smtpHost = s.value(QStringLiteral("smtpHost")).toString();
    m_account.smtpPort = s.value(QStringLiteral("smtpPort"), 465).toInt();
    m_account.smtpSecurity = s.value(QStringLiteral("smtpSecurity"), QStringLiteral("ssl")).toString();

    connect(&m_imap, &Imap::busyChanged, this, &MailStore::busyChanged);
    connect(&m_smtp, &Smtp::busyChanged, this, &MailStore::busyChanged);
    connect(&m_imap, &Imap::failed, this, &MailStore::failed);
    connect(&m_smtp, &Smtp::failed, this, &MailStore::failed);
    connect(&m_smtp, &Smtp::sent, this, &MailStore::sent);
    connect(&m_imap, &Imap::listed, this, [this](const QString &folder, const QVariantList &messages,
                                                 const QStringList &folders) {
        if (!folders.isEmpty() && folders != m_folders) {
            m_folders = folders;
            emit foldersChanged();
        }
        if (folder == m_folder) {
            m_messages = messages;
            emit messagesChanged();
        }
        saveCache();
    });
    connect(&m_imap, &Imap::fetched, this, [this](const QString &folder, uint uid, const QByteArray &raw) {
        QFile f(m_cacheDir + QStringLiteral("/bodies/") + cacheFile(folder) + QLatin1Char('-')
                + QString::number(uid) + QStringLiteral(".eml"));
        if (f.open(QIODevice::WriteOnly))
            f.write(raw);
        if (folder == m_folder) {
            setSeenLocally(uid, true);
            emit opened(uid, readMessage(uid, raw));
        }
    });
    loadCache();
}

QString MailStore::cacheFile(const QString &name) const
{
    QString s = name;
    for (QChar &c : s)
        if (!c.isLetterOrNumber())
            c = QLatin1Char('_');
    return s;
}

void MailStore::loadCache()
{
    QFile f(m_cacheDir + QStringLiteral("/") + cacheFile(m_folder) + QStringLiteral(".json"));
    m_messages.clear();
    if (f.open(QIODevice::ReadOnly))
        m_messages = QJsonDocument::fromJson(f.readAll()).array().toVariantList();
    for (QVariant &v : m_messages) {
        QVariantMap m = v.toMap();
        m[QStringLiteral("date")] = QDateTime::fromString(m.value(QStringLiteral("date")).toString(), Qt::ISODate);
        v = m;
    }
    QFile ff(m_cacheDir + QStringLiteral("/folders.json"));
    if (ff.open(QIODevice::ReadOnly))
        m_folders = QJsonDocument::fromJson(ff.readAll()).toVariant().toStringList();
    emit messagesChanged();
    emit foldersChanged();
}

void MailStore::saveCache() const
{
    QJsonArray a;
    for (const QVariant &v : m_messages) {
        QVariantMap m = v.toMap();
        m[QStringLiteral("date")] = m.value(QStringLiteral("date")).toDateTime().toString(Qt::ISODate);
        m.remove(QStringLiteral("received"));
        a.append(QJsonObject::fromVariantMap(m));
    }
    QFile f(m_cacheDir + QStringLiteral("/") + cacheFile(m_folder) + QStringLiteral(".json"));
    if (f.open(QIODevice::WriteOnly))
        f.write(QJsonDocument(a).toJson(QJsonDocument::Compact));
    QFile ff(m_cacheDir + QStringLiteral("/folders.json"));
    if (ff.open(QIODevice::WriteOnly))
        ff.write(QJsonDocument::fromVariant(m_folders).toJson(QJsonDocument::Compact));
}

int MailStore::unread() const
{
    int n = 0;
    for (const QVariant &v : m_messages)
        if (!v.toMap().value(QStringLiteral("seen")).toBool())
            ++n;
    return n;
}

static QString titleOf(const QString &folder)
{
    if (folder.compare(QLatin1String("INBOX"), Qt::CaseInsensitive) == 0)
        return QObject::tr("Inbox");
    QString name = Imap::folderName(folder.toLatin1());
    const int slash = qMax(name.lastIndexOf(QLatin1Char('/')), name.lastIndexOf(QLatin1Char('.')));
    return slash >= 0 && slash + 1 < name.size() ? name.mid(slash + 1) : name;
}

QString MailStore::folderTitle() const
{
    return titleOf(m_folder);
}

QVariantList MailStore::folders() const
{
    QVariantList list;
    QStringList names = m_folders.isEmpty() ? QStringList{QStringLiteral("INBOX")} : m_folders;
    // the inbox first
    std::stable_sort(names.begin(), names.end(), [](const QString &a, const QString &b) {
        return a.compare(QLatin1String("INBOX"), Qt::CaseInsensitive) == 0
               && b.compare(QLatin1String("INBOX"), Qt::CaseInsensitive) != 0;
    });
    for (const QString &f : names) {
        const QString l = f.toLower();
        const QString icon = l == QLatin1String("inbox") ? QStringLiteral("icon-m-email-directory-incoming")
            : l.contains(QLatin1String("sent")) || l.contains(QLatin1String("gesendet")) ? QStringLiteral("icon-m-email-directory-send")
            : l.contains(QLatin1String("draft")) || l.contains(QLatin1String("entw")) ? QStringLiteral("icon-m-email-directory-draft")
            : l.contains(QLatin1String("trash")) || l.contains(QLatin1String("papierkorb")) || l.contains(QLatin1String("deleted"))
                ? QStringLiteral("icon-m-email-directory-trash")
            : l.contains(QLatin1String("spam")) || l.contains(QLatin1String("junk")) ? QStringLiteral("icon-m-email-directory-spam")
            : QStringLiteral("icon-m-email-mailbox");
        list << QVariantMap{{QStringLiteral("name"), f}, {QStringLiteral("title"), titleOf(f)},
                            {QStringLiteral("icon"), icon}};
    }
    return list;
}

QVariantMap MailStore::guess(const QString &address) const
{
    const QString domain = address.section(QLatin1Char('@'), 1).trimmed().toLower();
    struct P { const char *domains, *imap; int imapPort; const char *smtp; int smtpPort; const char *smtpSec; };
    static const P known[] = {
        {"gmail.com googlemail.com", "imap.gmail.com", 993, "smtp.gmail.com", 465, "ssl"},
        {"outlook.com hotmail.com live.com msn.com", "outlook.office365.com", 993, "smtp.office365.com", 587, "starttls"},
        {"gmx.at gmx.de gmx.net gmx.ch", "imap.gmx.net", 993, "mail.gmx.net", 465, "ssl"},
        {"web.de", "imap.web.de", 993, "smtp.web.de", 587, "starttls"},
        {"posteo.de posteo.net", "posteo.de", 993, "posteo.de", 465, "ssl"},
        {"mailbox.org", "imap.mailbox.org", 993, "smtp.mailbox.org", 465, "ssl"},
        {"yahoo.com yahoo.de", "imap.mail.yahoo.com", 993, "smtp.mail.yahoo.com", 465, "ssl"},
        {"icloud.com me.com mac.com", "imap.mail.me.com", 993, "smtp.mail.me.com", 587, "starttls"},
        {"protonmail.com proton.me", "127.0.0.1", 1143, "127.0.0.1", 1025, "starttls"},
    };
    for (const P &p : known) {
        if (QString::fromLatin1(p.domains).split(QLatin1Char(' ')).contains(domain))
            return QVariantMap{{QStringLiteral("imapHost"), QString::fromLatin1(p.imap)},
                               {QStringLiteral("imapPort"), p.imapPort},
                               {QStringLiteral("imapSecurity"), p.imapPort == 993 ? QStringLiteral("ssl") : QStringLiteral("starttls")},
                               {QStringLiteral("smtpHost"), QString::fromLatin1(p.smtp)},
                               {QStringLiteral("smtpPort"), p.smtpPort},
                               {QStringLiteral("smtpSecurity"), QString::fromLatin1(p.smtpSec)}};
    }
    if (domain.isEmpty())
        return QVariantMap();
    return QVariantMap{{QStringLiteral("imapHost"), QStringLiteral("imap.") + domain},
                       {QStringLiteral("imapPort"), 993}, {QStringLiteral("imapSecurity"), QStringLiteral("ssl")},
                       {QStringLiteral("smtpHost"), QStringLiteral("smtp.") + domain},
                       {QStringLiteral("smtpPort"), 465}, {QStringLiteral("smtpSecurity"), QStringLiteral("ssl")}};
}

QVariantMap MailStore::account() const
{
    return QVariantMap{{QStringLiteral("name"), m_account.name}, {QStringLiteral("address"), m_account.address},
                       {QStringLiteral("user"), m_account.user}, {QStringLiteral("password"), m_account.password},
                       {QStringLiteral("imapHost"), m_account.imapHost}, {QStringLiteral("imapPort"), m_account.imapPort},
                       {QStringLiteral("imapSecurity"), m_account.imapSecurity},
                       {QStringLiteral("smtpHost"), m_account.smtpHost}, {QStringLiteral("smtpPort"), m_account.smtpPort},
                       {QStringLiteral("smtpSecurity"), m_account.smtpSecurity}};
}

void MailStore::setup(const QVariantMap &a)
{
    m_account.name = a.value(QStringLiteral("name")).toString();
    m_account.address = a.value(QStringLiteral("address")).toString().trimmed();
    m_account.user = a.value(QStringLiteral("user")).toString().trimmed();
    if (m_account.user.isEmpty())
        m_account.user = m_account.address;
    m_account.password = a.value(QStringLiteral("password")).toString();
    m_account.imapHost = a.value(QStringLiteral("imapHost")).toString().trimmed();
    m_account.imapPort = a.value(QStringLiteral("imapPort"), 993).toInt();
    m_account.imapSecurity = a.value(QStringLiteral("imapSecurity"), QStringLiteral("ssl")).toString();
    m_account.smtpHost = a.value(QStringLiteral("smtpHost")).toString().trimmed();
    m_account.smtpPort = a.value(QStringLiteral("smtpPort"), 465).toInt();
    m_account.smtpSecurity = a.value(QStringLiteral("smtpSecurity"), QStringLiteral("ssl")).toString();

    QSettings s(QStringLiteral("meego"), QStringLiteral("meego-mail"));
    const QVariantMap stored = account();
    for (auto it = stored.begin(); it != stored.end(); ++it)
        s.setValue(it.key(), it.value());
    s.sync();
    // the password is in there: for the user alone
    QFile::setPermissions(s.fileName(), QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    m_folders.clear();
    m_folder = QStringLiteral("INBOX");
    m_messages.clear();
    emit accountChanged();
    emit folderChanged();
    emit messagesChanged();
    refresh();
}

void MailStore::refresh()
{
    if (configured())
        m_imap.list(m_account, m_folder, listSize);
}

void MailStore::openFolder(const QString &folder)
{
    if (folder == m_folder)
        return;
    m_folder = folder;
    emit folderChanged();
    loadCache();
    refresh();
}

QVariantMap MailStore::readMessage(uint uid, const QByteArray &raw) const
{
    const Mime::Message m = Mime::parse(raw);
    QVariantMap header;
    for (const QVariant &v : m_messages)
        if (v.toMap().value(QStringLiteral("uid")).toUInt() == uid)
            header = v.toMap();
    return QVariantMap{
        {QStringLiteral("uid"), uid},
        {QStringLiteral("subject"), m.subject.isEmpty() ? header.value(QStringLiteral("subject")).toString() : m.subject},
        {QStringLiteral("from"), m.from}, {QStringLiteral("to"), m.to}, {QStringLiteral("cc"), m.cc},
        {QStringLiteral("date"), m.date.isValid() ? m.date : header.value(QStringLiteral("date")).toDateTime()},
        {QStringLiteral("text"), m.text}, {QStringLiteral("html"), m.html},
        {QStringLiteral("attachments"), m.attachments},
        {QStringLiteral("fromAddress"), header.value(QStringLiteral("fromAddress"))},
        {QStringLiteral("messageId"), header.value(QStringLiteral("messageId"))},
    };
}

void MailStore::open(uint uid)
{
    QFile f(m_cacheDir + QStringLiteral("/bodies/") + cacheFile(m_folder) + QLatin1Char('-')
            + QString::number(uid) + QStringLiteral(".eml"));
    if (f.open(QIODevice::ReadOnly)) {
        emit opened(uid, readMessage(uid, f.readAll()));
        bool seen = true;
        for (const QVariant &v : m_messages)
            if (v.toMap().value(QStringLiteral("uid")).toUInt() == uid)
                seen = v.toMap().value(QStringLiteral("seen")).toBool();
        if (!seen) {
            setSeenLocally(uid, true);
            m_imap.setSeen(m_account, m_folder, uid, true);
        }
        return;
    }
    m_imap.fetch(m_account, m_folder, uid);
}

void MailStore::setSeenLocally(uint uid, bool seen)
{
    for (QVariant &v : m_messages) {
        QVariantMap m = v.toMap();
        if (m.value(QStringLiteral("uid")).toUInt() == uid) {
            m[QStringLiteral("seen")] = seen;
            v = m;
        }
    }
    saveCache();
    emit messagesChanged();
}

void MailStore::markUnread(uint uid)
{
    setSeenLocally(uid, false);
    m_imap.setSeen(m_account, m_folder, uid, false);
}

void MailStore::remove(uint uid)
{
    for (int i = 0; i < m_messages.size(); ++i)
        if (m_messages.at(i).toMap().value(QStringLiteral("uid")).toUInt() == uid)
            m_messages.removeAt(i--);
    saveCache();
    emit messagesChanged();
    m_imap.remove(m_account, m_folder, uid);
}

static QStringList addressesOf(const QString &field)
{
    QStringList out;
    for (QString a : field.split(QRegularExpression(QStringLiteral("[,;]")), Qt::SkipEmptyParts)) {
        a = a.trimmed();
        const int lt = a.indexOf(QLatin1Char('<')), gt = a.indexOf(QLatin1Char('>'));
        out << (lt >= 0 && gt > lt ? a.mid(lt + 1, gt - lt - 1) : a);
    }
    return out;
}

void MailStore::send(const QString &to, const QString &cc, const QString &subject,
                     const QString &body, const QString &inReplyTo)
{
    const QString from = m_account.name.isEmpty() ? m_account.address
                                                  : m_account.name + QStringLiteral(" <") + m_account.address + QLatin1Char('>');
    const QStringList toList = to.split(QRegularExpression(QStringLiteral("[,;]")), Qt::SkipEmptyParts);
    const QStringList ccList = cc.split(QRegularExpression(QStringLiteral("[,;]")), Qt::SkipEmptyParts);
    const QByteArray message = Mime::compose(from, toList, ccList, subject, body, inReplyTo);
    m_smtp.send(m_account, addressesOf(to) + addressesOf(cc), message);
}
