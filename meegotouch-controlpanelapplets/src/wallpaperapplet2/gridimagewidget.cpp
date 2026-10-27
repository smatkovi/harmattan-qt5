
#include "gridimagewidget.h"
#include <QGraphicsSceneMouseEvent>

#include <MImageWidget>
#include <MProgressIndicator>
#include <QPen>
#include <QPropertyAnimation>

#include <MTheme>
#include <MStyle>
#include <mbuttoniconstyle.h>

//#define DEBUG
//#define WARNING
#include "../debug.h"

#include <MWidgetCreator>
M_REGISTER_WIDGET_NO_CREATE(GridImageWidget)

//static const QString progressType = MProgressIndicator::barType;
static const QString progressType = MProgressIndicator::spinnerType;
static const QPen selectionPen (
        QBrush(), 4, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);


GridImageLayout::GridImageLayout (
        QGraphicsLayoutItem *parent) : 
    QGraphicsLayout (parent),
    m_Image (0),
    m_ProgressBar (0)
{
}
 
GridImageLayout::~GridImageLayout()
{
#if 0
      QLayoutItem *item;
      while ((item = takeAt(0)))
          delete item;
#endif
}


int 
GridImageLayout::count() const
{
    int retval = 0;

    if (m_Image)
        retval++;
    if (m_ProgressBar)
        retval++;

    //SYS_DEBUG ("Returning %d", retval);
    return retval;
}


QGraphicsLayoutItem *
GridImageLayout::itemAt (int idx) const
{
    QGraphicsLayoutItem *retval = 0;

    if (idx == 0)
        retval = m_Image;
    else if (idx == 1)
        retval = m_ProgressBar;

    //SYS_DEBUG ("Returning %p at %d", retval, idx);
    return retval;
}

QGraphicsLayoutItem *
GridImageLayout::takeAt (int idx)
{
    SYS_DEBUG ("");
    return idx >= 0 && idx < list.size() ? list.takeAt(idx) : 0;
}

void 
GridImageLayout::removeAt (
        int index)
{
    SYS_DEBUG ("");
    list.removeAt (index);
    invalidate ();
}


void 
GridImageLayout::addItem (
        QGraphicsLayoutItem *layoutItem,
        GridImageLayoutRole  role)
{
    switch (role) {
        case Image:
            SYS_DEBUG ("Adding image");
            m_Image = layoutItem;
            break;

        case ProgressIndicator:
            SYS_DEBUG ("Adding progress indicator");
            m_ProgressBar = layoutItem;
            break;
    }

    addChildLayoutItem (layoutItem);
    invalidate ();
}

void 
GridImageLayout::setGeometry(
        const QRectF &r)
{
    qreal left, top, right, bottom;

    QGraphicsLayout::setGeometry(r);

    getContentsMargins (&left, &top, &right, &bottom);

    if (m_Image)
        m_Image->setGeometry (r);

    if (m_ProgressBar) {
        QSizeF progressSize;
        QRectF progressGeometry;
        
        if (progressType == MProgressIndicator::barType) {
            progressSize = m_ProgressBar->preferredSize ();
            progressGeometry = QRectF (
                    left, 
                    r.y() + r.height () - progressSize.height () - bottom, 
                    r.width () - left - right, 
                    progressSize.height ());
        } else {
            qreal HRem, VRem;
            progressSize = m_ProgressBar->minimumSize ();
            qreal progrWidth = progressSize.width();
            qreal progrHeight = progressSize.height();

            HRem = (r.width() - left - right - progrWidth) / 2;
            VRem = (r.height() - top - bottom - progrHeight) / 2;

            progressGeometry = QRectF (
                    left + HRem, 
                    top + VRem, 
                    progrWidth, 
                    progrHeight);
        }

        SYS_DEBUG ("*** progressSize = %gx%g", 
                progressSize.width(), 
                progressSize.height());

        SYS_DEBUG ("*** progressGeometry = %g, %g (%gx%g)", 
                progressGeometry.x(), 
                progressGeometry.y(), 
                progressGeometry.width(), 
                progressGeometry.height());
        m_ProgressBar->setGeometry (progressGeometry);
     }
}


QSizeF 
GridImageLayout::sizeHint (
        Qt::SizeHint which, 
        const QSizeF &constraint) const
{
    Q_UNUSED (which);
    Q_UNUSED (constraint);
    QSizeF retval (160.0, 160.0);

    if (m_Image)
        return m_Image->preferredSize ();

    return retval;
}



/******************************************************************************
 * GridImageWidget implementation.
 */
GridImageWidget::GridImageWidget () :
    m_Layout (0),
    m_ProgressIndicator (0),
    m_Current (false)
{
    m_OpacityAnimation = new QPropertyAnimation(this, "opacity", this);
    m_OpacityAnimation->setStartValue(0.0);
    m_OpacityAnimation->setEndValue(1.0);
    m_OpacityAnimation->setDuration(250); 

    createLayout ();
}

GridImageWidget::~GridImageWidget ()
{
}

void 
GridImageWidget::setCurrent (
        bool current)
{
    m_Current = current;
}

void 
GridImageWidget::paint (
        QPainter                       *painter, 
        const QStyleOptionGraphicsItem *option, 
        QWidget                        *widget)
{
    QRectF geom = geometry ();

    Q_UNUSED (option);
    Q_UNUSED (widget);

    //MListItem::paint (painter, option, widget);

    if (m_ProgressIndicator && m_ProgressIndicator->isVisible())
        painter->setOpacity (0.5);

    painter->drawPixmap (
            0, 0, (int) geom.width(), (int) geom.height(), m_Pixmap);

    if (m_Current) {
        QPen pen(selectionPen);

        /*
         * FIXME: Somehow this theming does not work. This is the default color,
         * but we should use theming here.
         * MButtonIconStyle *m_HighlightStyle ->glowColor()
         */
        pen.setColor (QColor("#4292ff"));
        painter->setPen(pen);

        const int mod    = pen.width() % 2;
        const int adjust = pen.width() / 2;
        QRect rect (0, 0, 
                (int) geom.width(), (int) geom.height());
        const QRect selection = rect.adjusted (
                adjust, adjust, -(adjust + mod), -(adjust + mod));
        painter->drawRect(selection);
    }
}

void 
GridImageWidget::setPixmap(
        const QPixmap &pixmap)
{
    SYS_DEBUG ("");

    if (!m_Layout)
        createLayout ();

    m_Pixmap = pixmap;
    update ();
}

void
GridImageWidget::setID (
        const QString &id)
{
    m_ID = id;
}

QString 
GridImageWidget::id () const
{
    return m_ID;
}

void 
GridImageWidget::setProgress (
        bool showProgress)
{
    SYS_DEBUG ("*** showProgress = %s", SYS_BOOL(showProgress));
    if (showProgress) {
        progressIndicator(true)->show();
    } else if (m_ProgressIndicator) {
        m_ProgressIndicator->hide();
    }
}

MProgressIndicator *
GridImageWidget::progressIndicator (
        bool create)
{
    if (!m_Layout)
        createLayout ();

    if (!m_ProgressIndicator && create) {
        SYS_DEBUG ("Creating a progress indicator.");
        m_ProgressIndicator = new MProgressIndicator (this, progressType);
        m_ProgressIndicator->setObjectName ("WallpaperLoadingSpinner");
        m_ProgressIndicator->setStyleName ("CommonLargeSpinnerInverted");
        m_ProgressIndicator->setUnknownDuration (true);
        m_Layout->addItem (
                m_ProgressIndicator, 
                GridImageLayout::ProgressIndicator);
        // This seems to solve the issue in NB#208329. FIXME: We should at least
        // use some stored/calculated values instead of these litarals.
        m_Layout->setGeometry (QRectF(0.0, 0.0, 160.0, 160.0));
    } else {
        SYS_DEBUG ("Already have a progress indicator.");
    }

    return m_ProgressIndicator;
}


void 
GridImageWidget::createLayout()
{
    if (m_Layout) {
        SYS_WARNING ("Layout already created.");
        return;
    }

    m_Layout = new GridImageLayout(this);
    m_Layout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    setLayout (m_Layout);
}

void GridImageWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    MListItem::mousePressEvent(event);
    event->accept();
}

void GridImageWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    MListItem::mouseReleaseEvent(event);
    emit clicked();
}

void GridImageWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    MListItem::contextMenuEvent(event);
    emit longPressed();
}

void 
GridImageWidget::showAnimated()
{
    m_OpacityAnimation->start();
}
