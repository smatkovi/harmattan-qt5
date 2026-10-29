// The notes: one plain text file each (UTF-8, the first line its title)
// in ~/.local/share/meego/meego-notes/notes, the file's time the note's;
// changes from outside (a sync, an editor) are seen at once.
#ifndef NOTESTORE_H
#define NOTESTORE_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QVariantList>

class NoteStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList notes READ notes NOTIFY changed)
    Q_PROPERTY(QString folder READ folder CONSTANT)

public:
    explicit NoteStore(QObject *parent = nullptr);

    QVariantList notes() const { return m_notes; }
    QString folder() const { return m_dir; }

    // the notes containing all words of the filter, the newest first
    Q_INVOKABLE QVariantList find(const QString &filter) const;
    Q_INVOKABLE QString text(const QString &id) const;
    // a new note for an empty id; an empty text removes it; the note's id
    Q_INVOKABLE QString save(const QString &id, const QString &text);
    Q_INVOKABLE bool remove(const QString &id);

signals:
    void changed();

private:
    void reload();
    QString path(const QString &id) const;

    QString m_dir;
    QVariantList m_notes;
    QFileSystemWatcher m_watcher;
};

#endif
