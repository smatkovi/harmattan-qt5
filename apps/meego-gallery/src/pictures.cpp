#include "pictures.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImageReader>
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>
#include <algorithm>

static QVariantList scan(const QStringList &roots)
{
    QVariantList list;
    QSet<QString> seen;
    const QStringList filters{QStringLiteral("*.jpg"), QStringLiteral("*.jpeg"), QStringLiteral("*.png"),
                              QStringLiteral("*.gif"), QStringLiteral("*.bmp"), QStringLiteral("*.webp"),
                              QStringLiteral("*.JPG"), QStringLiteral("*.JPEG"), QStringLiteral("*.PNG")};
    for (const QString &root : roots) {
        QDirIterator it(root, filters, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QFileInfo fi(it.next());
            const QString path = fi.canonicalFilePath();
            // hidden folders (thumbnail caches) are not albums
            if (path.isEmpty() || seen.contains(path) || path.contains(QLatin1String("/.")))
                continue;
            seen.insert(path);
            list << QVariantMap{{QStringLiteral("url"), QUrl::fromLocalFile(path).toString()},
                                {QStringLiteral("path"), path},
                                {QStringLiteral("name"), fi.fileName()},
                                {QStringLiteral("folder"), fi.absolutePath()},
                                {QStringLiteral("time"), fi.lastModified()},
                                {QStringLiteral("size"), fi.size()}};
        }
    }
    std::sort(list.begin(), list.end(), [](const QVariant &a, const QVariant &b) {
        return a.toMap().value(QStringLiteral("time")).toDateTime() > b.toMap().value(QStringLiteral("time")).toDateTime();
    });
    return list;
}

Pictures::Pictures(QObject *parent) : QObject(parent)
{
    const QString home = QDir::homePath();
    m_roots << QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
            << home + QStringLiteral("/DCIM") << home + QStringLiteral("/MyDocs/DCIM");
    if (qEnvironmentVariableIsSet("MEEGO_GALLERY_DIR"))
        m_roots = qEnvironmentVariable("MEEGO_GALLERY_DIR").split(QLatin1Char(':'));
    connect(&m_watcher, &QFutureWatcher<QVariantList>::finished, this, [this] {
        m_all = m_watcher.result();
        emit scanningChanged();
        emit changed();
    });
    rescan();
}

void Pictures::rescan()
{
    if (m_watcher.isRunning())
        return;
    m_watcher.setFuture(QtConcurrent::run(scan, m_roots));
    emit scanningChanged();
}

QVariantList Pictures::albums() const
{
    QMap<QString, QVariantMap> by;
    for (const QVariant &v : m_all) {
        const QVariantMap p = v.toMap();
        const QString folder = p.value(QStringLiteral("folder")).toString();
        QVariantMap &a = by[folder];
        if (a.isEmpty()) {
            a[QStringLiteral("folder")] = folder;
            a[QStringLiteral("name")] = QFileInfo(folder).fileName();
            a[QStringLiteral("cover")] = p.value(QStringLiteral("url"));   // the newest
        }
        a[QStringLiteral("count")] = a.value(QStringLiteral("count")).toInt() + 1;
    }
    QVariantList list;
    for (const QVariantMap &a : by)
        list << a;
    return list;
}

QVariantList Pictures::inFolder(const QString &folder) const
{
    QVariantList list;
    for (const QVariant &v : m_all)
        if (v.toMap().value(QStringLiteral("folder")).toString() == folder)
            list << v;
    return list;
}

int Pictures::indexOf(const QString &url) const
{
    for (int i = 0; i < m_all.size(); ++i)
        if (m_all.at(i).toMap().value(QStringLiteral("url")).toString() == url)
            return i;
    return -1;
}

bool Pictures::remove(const QString &path)
{
    QFile f(path);
    if (!f.moveToTrash() && !f.remove())
        return false;
    for (int i = 0; i < m_all.size(); ++i)
        if (m_all.at(i).toMap().value(QStringLiteral("path")).toString() == path)
            m_all.removeAt(i--);
    emit changed();
    return true;
}

QSize Pictures::dimensions(const QString &path) const
{
    return QImageReader(path).size();
}
