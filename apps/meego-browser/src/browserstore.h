// The browser's bookmarks and history, as JSON in
// ~/.local/share/meego/meego-browser; and what the address field's text
// means (an address, a file, else a search).
#ifndef BROWSERSTORE_H
#define BROWSERSTORE_H

#include <QObject>
#include <QVariantList>

class BrowserStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList bookmarks READ bookmarks NOTIFY changed)
    Q_PROPERTY(QVariantList history READ history NOTIFY changed)

public:
    explicit BrowserStore(QObject *parent = nullptr);

    QVariantList bookmarks() const { return m_bookmarks; }
    QVariantList history() const { return m_history; }

    Q_INVOKABLE QString fixUrl(const QString &text) const;
    // a page seen (again: to the top, its newer title)
    Q_INVOKABLE void visit(const QString &url, const QString &title);
    Q_INVOKABLE bool isBookmarked(const QString &url) const;
    Q_INVOKABLE void toggleBookmark(const QString &url, const QString &title);
    Q_INVOKABLE void removeBookmark(const QString &url);
    Q_INVOKABLE void clearHistory();

signals:
    void changed();

private:
    void save(const QString &name, const QVariantList &list) const;
    QVariantList load(const QString &name) const;

    QString m_dir;
    QVariantList m_bookmarks, m_history;
};

#endif
