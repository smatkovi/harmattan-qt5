// The videos below the videos folder (and the camera's), the newest
// first, with a picture of each (a frame a tenth into it, cached) and
// its length.
#ifndef VIDEOLIBRARY_H
#define VIDEOLIBRARY_H

#include <QFutureWatcher>
#include <QObject>
#include <QVariantList>

class VideoLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantList videos READ videos NOTIFY changed)

public:
    explicit VideoLibrary(QObject *parent = nullptr);

    bool scanning() const { return m_watcher.isRunning(); }
    // each: url, path, title, time, duration (ms), thumbnail (url or "")
    QVariantList videos() const { return m_videos; }
    Q_INVOKABLE void rescan();
    Q_INVOKABLE bool remove(const QString &path);

signals:
    void changed();
    void scanningChanged();

private:
    QStringList m_roots;
    QString m_cacheDir;
    QVariantList m_videos;
    QFutureWatcher<QVariantList> m_watcher;
};

#endif
