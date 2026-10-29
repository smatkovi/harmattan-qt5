#include "browserstore.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUrl>

static const int MAX_HISTORY = 300;

BrowserStore::BrowserStore(QObject *parent) : QObject(parent)
{
    m_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(m_dir);
    m_bookmarks = load(QStringLiteral("bookmarks.json"));
    m_history = load(QStringLiteral("history.json"));
}

QVariantList BrowserStore::load(const QString &name) const
{
    QFile f(m_dir + QLatin1Char('/') + name);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QJsonDocument::fromJson(f.readAll()).array().toVariantList();
}

void BrowserStore::save(const QString &name, const QVariantList &list) const
{
    QSaveFile f(m_dir + QLatin1Char('/') + name);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(QJsonArray::fromVariantList(list)).toJson(QJsonDocument::Compact));
        f.commit();
    }
}

QString BrowserStore::fixUrl(const QString &text) const
{
    const QString t = text.trimmed();
    if (t.isEmpty())
        return QString();
    // a scheme: as it is
    static const QStringList schemes{QStringLiteral("http:"), QStringLiteral("https:"), QStringLiteral("file:"),
                                     QStringLiteral("about:"), QStringLiteral("data:"), QStringLiteral("ftp:")};
    for (const QString &s : schemes)
        if (t.startsWith(s, Qt::CaseInsensitive))
            return t;
    if (t.startsWith(QLatin1Char('/')) && QFileInfo::exists(t))
        return QUrl::fromLocalFile(t).toString();
    // something.like/this, localhost:8080: an address
    if (!t.contains(QLatin1Char(' ')) && (t.contains(QLatin1Char('.')) || t.startsWith(QLatin1String("localhost"))))
        return QStringLiteral("https://") + t;
    QString search = QStringLiteral("https://duckduckgo.com/?q=%s");
    if (qEnvironmentVariableIsSet("MEEGO_BROWSER_SEARCH"))
        search = qEnvironmentVariable("MEEGO_BROWSER_SEARCH");
    return search.replace(QLatin1String("%s"), QString::fromLatin1(QUrl::toPercentEncoding(t)));
}

void BrowserStore::visit(const QString &url, const QString &title)
{
    if (url.isEmpty() || url.startsWith(QLatin1String("about:")) || url.startsWith(QLatin1String("data:")))
        return;
    for (int i = 0; i < m_history.size(); ++i)
        if (m_history.at(i).toMap().value(QStringLiteral("url")).toString() == url)
            m_history.removeAt(i--);
    m_history.prepend(QVariantMap{{QStringLiteral("url"), url},
                                  {QStringLiteral("title"), title.isEmpty() ? url : title},
                                  {QStringLiteral("time"), QDateTime::currentDateTime().toString(Qt::ISODate)}});
    while (m_history.size() > MAX_HISTORY)
        m_history.removeLast();
    // a bookmark's title follows the page's
    for (int i = 0; i < m_bookmarks.size(); ++i) {
        QVariantMap b = m_bookmarks.at(i).toMap();
        if (b.value(QStringLiteral("url")).toString() == url && !title.isEmpty() && b.value(QStringLiteral("title")) != title) {
            b.insert(QStringLiteral("title"), title);
            m_bookmarks[i] = b;
            save(QStringLiteral("bookmarks.json"), m_bookmarks);
        }
    }
    save(QStringLiteral("history.json"), m_history);
    emit changed();
}

bool BrowserStore::isBookmarked(const QString &url) const
{
    for (const QVariant &v : m_bookmarks)
        if (v.toMap().value(QStringLiteral("url")).toString() == url)
            return true;
    return false;
}

void BrowserStore::toggleBookmark(const QString &url, const QString &title)
{
    if (url.isEmpty())
        return;
    if (isBookmarked(url))
        return removeBookmark(url);
    m_bookmarks.append(QVariantMap{{QStringLiteral("url"), url}, {QStringLiteral("title"), title.isEmpty() ? url : title}});
    save(QStringLiteral("bookmarks.json"), m_bookmarks);
    emit changed();
}

void BrowserStore::removeBookmark(const QString &url)
{
    for (int i = 0; i < m_bookmarks.size(); ++i)
        if (m_bookmarks.at(i).toMap().value(QStringLiteral("url")).toString() == url)
            m_bookmarks.removeAt(i--);
    save(QStringLiteral("bookmarks.json"), m_bookmarks);
    emit changed();
}

void BrowserStore::clearHistory()
{
    m_history.clear();
    save(QStringLiteral("history.json"), m_history);
    emit changed();
}
