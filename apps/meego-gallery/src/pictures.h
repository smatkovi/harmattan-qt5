// The pictures: the image files below the pictures folder, the camera's
// (DCIM) and MEEGO_GALLERY_DIR, the newest first, and their albums
// (folders); found in the background.
#ifndef PICTURES_H
#define PICTURES_H

#include <QFutureWatcher>
#include <QObject>
#include <QSize>
#include <QVariantList>

class Pictures : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantList all READ all NOTIFY changed)
    Q_PROPERTY(QVariantList albums READ albums NOTIFY changed)

public:
    explicit Pictures(QObject *parent = nullptr);

    bool scanning() const { return m_watcher.isRunning(); }
    // each: url, path, name, folder, time, size (bytes)
    QVariantList all() const { return m_all; }
    // each: name, folder, count, cover (url)
    QVariantList albums() const;
    Q_INVOKABLE QVariantList inFolder(const QString &folder) const;
    Q_INVOKABLE int indexOf(const QString &url) const;
    Q_INVOKABLE void rescan();
    // into the desktop's trash; true when gone
    Q_INVOKABLE bool remove(const QString &path);
    // width and height of a picture, read on demand
    Q_INVOKABLE QSize dimensions(const QString &path) const;

signals:
    void changed();
    void scanningChanged();

private:
    QStringList m_roots;
    QVariantList m_all;
    QFutureWatcher<QVariantList> m_watcher;
};

#endif
