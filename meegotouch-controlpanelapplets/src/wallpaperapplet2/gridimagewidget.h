#ifndef GRIDIMAGEWIDGET_H
#define GRIDIMAGEWIDGET_H

#include <MListItem>
#include <QString>

#include <QGraphicsLayout>
#include <QList>

class MImageWidget;
class MProgressIndicator;
class MButtonIconStyle;
class QPropertyAnimation;

class GridImageLayout : public QGraphicsLayout
{
    public:
        GridImageLayout (QGraphicsLayoutItem *parent = 0);
        ~GridImageLayout();

        typedef enum {
            Image,
            ProgressIndicator
        } GridImageLayoutRole;

        void addItem (
                QGraphicsLayoutItem *item, 
                GridImageLayoutRole role);
     
        QSizeF sizeHint (
             Qt::SizeHint which, 
             const QSizeF &constraint = QSizeF()) const;

        int count() const;

        QGraphicsLayoutItem *itemAt(int) const;
        QGraphicsLayoutItem *takeAt(int);
        void removeAt (int index);

        virtual void setGeometry(const QRectF &rect);

    private:
        QList<QGraphicsLayoutItem*> list;
        QGraphicsLayoutItem   *m_Image;
        QGraphicsLayoutItem   *m_ProgressBar;
};

class GridImageWidget : public MListItem
{
    Q_OBJECT

    public:
        GridImageWidget();
        ~GridImageWidget();
       
        void setCurrent (bool current);

        QString image() const;
        
        void setID (const QString &id);
        QString id () const;

        void setProgress (bool showProgress = true);
        void showAnimated();

    public Q_SLOTS:
        void setPixmap(const QPixmap &pixmap);
        MProgressIndicator *progressIndicator(bool create);

    Q_SIGNALS:
        void longPressed();

    protected:
        virtual void createLayout();
        virtual void paint (
                QPainter* painter, 
                const QStyleOptionGraphicsItem* option, 
                QWidget* widget = 0);

        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

   private:
        GridImageLayout        *m_Layout;
        MProgressIndicator     *m_ProgressIndicator;
        QPixmap                 m_Pixmap;
        bool                    m_Current;
        QString                 m_ID;
        QPropertyAnimation     *m_OpacityAnimation;

    friend class Ut_WallpaperGridImageWidget;
};
#endif
