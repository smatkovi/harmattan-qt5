#include "notestore.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <algorithm>

NoteStore::NoteStore(QObject *parent) : QObject(parent)
{
    m_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/notes");
    if (qEnvironmentVariableIsSet("MEEGO_NOTES_DIR"))
        m_dir = qEnvironmentVariable("MEEGO_NOTES_DIR");
    QDir().mkpath(m_dir);
    m_watcher.addPath(m_dir);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &NoteStore::reload);
    reload();
}

QString NoteStore::path(const QString &id) const
{
    return m_dir + QLatin1Char('/') + id + QStringLiteral(".txt");
}

void NoteStore::reload()
{
    QVariantList list;
    const QFileInfoList files = QDir(m_dir).entryInfoList({QStringLiteral("*.txt")}, QDir::Files);
    for (const QFileInfo &fi : files) {
        QFile f(fi.filePath());
        if (!f.open(QIODevice::ReadOnly))
            continue;
        const QString text = QString::fromUtf8(f.readAll());
        // the title: the first line with something on it; the rest below
        QString title, rest;
        const QStringList lines = text.split(QLatin1Char('\n'));
        int i = 0;
        while (i < lines.size() && lines.at(i).trimmed().isEmpty())
            ++i;
        if (i < lines.size())
            title = lines.at(i++).trimmed();
        rest = lines.mid(i).join(QLatin1Char('\n')).trimmed();
        list << QVariantMap{{QStringLiteral("id"), fi.completeBaseName()},
                            {QStringLiteral("text"), text},
                            {QStringLiteral("title"), title},
                            {QStringLiteral("rest"), rest},
                            {QStringLiteral("modified"), fi.lastModified()}};
    }
    std::sort(list.begin(), list.end(), [](const QVariant &a, const QVariant &b) {
        return a.toMap().value(QStringLiteral("modified")).toDateTime() > b.toMap().value(QStringLiteral("modified")).toDateTime();
    });
    m_notes = list;
    emit changed();
}

QVariantList NoteStore::find(const QString &filter) const
{
    const QStringList words = filter.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (words.isEmpty())
        return m_notes;
    QVariantList out;
    for (const QVariant &v : m_notes) {
        const QString text = v.toMap().value(QStringLiteral("text")).toString();
        if (std::all_of(words.begin(), words.end(), [&](const QString &w) { return text.contains(w, Qt::CaseInsensitive); }))
            out << v;
    }
    return out;
}

QString NoteStore::text(const QString &id) const
{
    QFile f(path(id));
    return f.open(QIODevice::ReadOnly) ? QString::fromUtf8(f.readAll()) : QString();
}

QString NoteStore::save(const QString &id, const QString &text)
{
    if (id.contains(QLatin1Char('/')))
        return QString();
    if (text.trimmed().isEmpty()) {
        if (!id.isEmpty())
            remove(id);
        return QString();
    }
    QString name = id;
    if (name.isEmpty()) {
        // the time of its making, unique
        const QString base = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-hhmmss"));
        name = base;
        for (int n = 2; QFile::exists(path(name)); ++n)
            name = base + QLatin1Char('-') + QString::number(n);
    } else if (this->text(name) == text) {
        return name;        // unchanged: its time stays
    }
    QSaveFile f(path(name));
    if (!f.open(QIODevice::WriteOnly) || f.write(text.toUtf8()) < 0 || !f.commit())
        return QString();
    reload();
    return name;
}

bool NoteStore::remove(const QString &id)
{
    if (id.isEmpty() || id.contains(QLatin1Char('/')))
        return false;
    QFile f(path(id));
    if (!f.moveToTrash() && !f.remove())
        return false;
    reload();
    return true;
}
