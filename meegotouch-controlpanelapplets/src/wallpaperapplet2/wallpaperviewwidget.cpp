/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#include "wallpaperviewwidget.h"
#include "wallpaperconfiguration.h"
#include "wallpaperutils.h"

#include <QTimer>
#include <QPinchGesture>
#include <QGraphicsSceneWheelEvent>

#include <MWindow>
#include <MApplication>
#include <MApplicationPage>
#include <MPannableViewport>
#include <MPositionIndicator>
#include <MWidgetStyle>
#include <MGConfItem>

//#define DEBUG
#define WARNING
#include "../debug.h"

#include <mwidgetcreator.h>
M_REGISTER_WIDGET_NO_CREATE(WallpaperViewWidget)


static const int ExtraMargin = 0;
static const int TitleBarHeight = 60;
static const int alwaysVisibleFromImage = 15;

static const qreal scaleLowerLimit = 0.2;
static const qreal scaleUpperLimit = 4.0;

/******************************************************************************
 *
 */
WallpaperViewWidget::WallpaperViewWidget (
        WallpaperBusinessLogic *wallpaperBusinessLogic, 
        QGraphicsWidget        *parent) :
    DcpStylableWidget (parent),
    m_BusinessLogic (wallpaperBusinessLogic),
    m_BgColor ("black"),
    m_Saving (false),
    m_Initialized (false),
    m_ImageWidget (0),
    m_PageRealized (false),
    m_PinchOngoing (false),
    m_Scaling (false),
    m_Rotating (false),
    m_PanOngoing (false),
    m_HasPendingRedraw (false),
    m_HasPendingSave (false),
    m_Physics (0),
    m_SupportPanEdit (false),
    m_SupportPinchEdit (false),
    m_FullScreen (false)
{
    MWindow *win = MApplication::activeWindow ();

    setObjectName ("WallpaperViewWidget");

    /*
     * FIXME: To clean up this...
     */
    if (win) {
        m_Orientation = win->orientation();
        m_OrientationLocked = win->isOrientationLocked ();
    } else {
        SYS_WARNING ("We have a problem: We don't know the orientation! "
                    "Only in tests though...");
        m_Orientation = M::Landscape;
    }

    /*
     * Enabling two finger gestures.
     */
    setAcceptTouchEvents (true);
    grabGesture (Qt::PinchGesture, Qt::GestureFlags());
    grabGesture (Qt::PanGesture, Qt::GestureFlags());

    connect (&m_RotateAnimation, SIGNAL(finished()),
            this, SLOT(rotateAnimationFinished()));

    connect (m_BusinessLogic, SIGNAL(wallpaperSaved()),
            this, SLOT(wallpaperSaved()));
    connect (m_BusinessLogic, SIGNAL(wallpaperLoaded (QuillImage, QSize)),
            this, SLOT(wallpaperLoaded (QuillImage, QSize)));
}

WallpaperViewWidget::~WallpaperViewWidget ()
{
    if (Wallpaper::useFullScreen) {
        MWindow *win = MApplication::activeWindow ();

        if (win)
            win->showNormal ();
    }
}

void 
WallpaperViewWidget::applyStyle()
{
    MGConfItem eggsGconfItem (EGGS_GCONF_KEY);

    DcpStylableWidget::applyStyle ();

    if (m_Physics) {
        SYS_WARNING ("applyStyle already called.");
        return;
    }

    /*
     * Creating the MPhysics2DPanning object. These values are copyed from the 
     * mpannablewidgetstyle.css CSS file.
     */
    m_Physics = new MPhysics2DPanning (this);
    m_Physics->setPanDirection (Qt::Vertical | Qt::Horizontal);
    m_Physics->setEnabled(true);
  
#if 0
    SYS_DEBUG ("*** PointerSpringK  = %g", style()->pointerSpringK());
    SYS_DEBUG ("*** Friction        = %g", style()->friction());
    SYS_DEBUG ("*** SlidingFriction = %g", style()->slidingFriction());
    SYS_DEBUG ("*** BorderSpringK   = %g", style()->borderSpringK());
    SYS_DEBUG ("*** BorderFriction  = %g", style()->borderFriction());
    SYS_DEBUG ("*** maximumvelocity = %g", style()->maximumVelocity());
#endif

    if (eggsGconfItem.value().toBool()) {
        m_SupportPanEdit   = true;
        m_SupportPinchEdit = true;
    } else {
        m_SupportPanEdit   = style()->supportPanEdit();
        m_SupportPinchEdit = style()->supportPinchEdit();
    }

    m_Physics->setPointerSpringK  (style()->pointerSpringK());
    m_Physics->setFriction        (style()->friction());
    m_Physics->setSlidingFriction (style()->slidingFriction());
    m_Physics->setBorderSpringK   (style()->borderSpringK());
    m_Physics->setBorderFriction  (style()->borderFriction());
    m_Physics->setMaximumVelocity (style()->maximumVelocity());

    connect (m_Physics, SIGNAL(positionChanged(const QPointF &)),
            this, SLOT(panningPhysicsPositionChanged(const QPointF &)));
    connect (m_Physics, SIGNAL(panningStopped()),
            this, SLOT(panningPhysicsPanningStopped()));

    m_BgColor = style()->imageBackgroundColor();
}

/******************************************************************************
 * Handling physics as they are changing.
 */
void 
WallpaperViewWidget::panningPhysicsPositionChanged(
        const QPointF    &position)
{
    m_UserOffset = position;
    redrawImage ();
}

void 
WallpaperViewWidget::panningPhysicsPanningStopped ()
{
    redrawImage ();

    if (m_HasPendingSave)
        saveImage ();
}

void 
WallpaperViewWidget::pinchStopped ()
{
    qreal rotation  = m_Trans.rotation();
    qreal rRotation = 0.0;
    int   tmp;

    tmp = rotation;
    tmp = tmp % 360;
    rotation = tmp;

    m_Trans.setRotation (rotation);

    /*
     * Where we should go...
     */
    for (qreal rounded = -360.0; rounded <= 360.0; rounded += 90.0) {
        if (rotation >= rounded - 45.0 && rotation <= rounded + 45.0) {
            rRotation = rounded;
            break;
        }
    }
    
    if (rotation != rRotation) {
        m_RotateAnimation.setTargetObject (m_ImageWidget);
        m_RotateAnimation.setPropertyName ("rotation");
        m_RotateAnimation.setEasingCurve(QEasingCurve::InOutCubic);
        m_RotateAnimation.setStartValue (rotation);
        m_RotateAnimation.setEndValue (rRotation);
        m_RotateAnimation.setDuration (500.0);
        m_RotateAnimation.start();
    } else { 
        setupPanningPhysics ();
    }

    queueRedrawImage ();

    if (m_HasPendingSave)
        saveImage ();
}

void
WallpaperViewWidget::rotateAnimationFinished ()
{
    qreal rotation = m_ImageWidget->rotation();

    if (qAbs (qAbs (rotation) - 360.0) < 0.1) {
        rotation = 0.0;
        m_ImageWidget->setRotation (rotation);
    }

    m_Trans.setRotation (rotation);

    setupPanningPhysics ();

    if (m_HasPendingSave)
        saveImage ();
}


/******************************************************************************
 * Image saving and loading.
 */
void
WallpaperViewWidget::saveImage ()
{
    #if 0
    SYS_DEBUG ("--------------------------------------------------------");
    SYS_DEBUG ("*** expectedSize = %s", SYS_SIZE(m_Trans.expectedSize()));
    SYS_DEBUG ("*** originalSize = %s", SYS_SIZE(m_OriginalSize));
    SYS_DEBUG ("*** offset       = %s", SYS_POINTF(m_Trans.offset()));
    SYS_DEBUG ("*** scale        = %g", m_Trans.scale());
    SYS_DEBUG ("*** rotation     = %g", m_Trans.rotation());
    SYS_DEBUG ("*** prop.running = %s",
            SYS_BOOL(m_RotateAnimation.state() == QAbstractAnimation::Running));
    SYS_DEBUG ("*** pann.running = %s", SYS_BOOL(m_Physics->inMotion()));
    #endif

    if (m_RotateAnimation.state() == QAbstractAnimation::Running ||
        m_Physics->inMotion()) {
        m_HasPendingSave = true;
        return;
    }

    if (m_Trans.noTransformation() && 
            m_Trans.expectedSize() == m_OriginalSize) {
        m_Saving = true;
        m_BusinessLogic->setWallpaper ();
    } else {
        QPixmap pixmap = generatePixmap (m_Trans);

        m_Saving = true;
        m_BusinessLogic->setWallpaper (pixmap);
    }

    /*
     * Notifying the business logic about the editing ended. It is important,
     * otherwise the businesslogic will reject the next edit start requests.
     */
    m_BusinessLogic->endEdit ();
    m_HasPendingSave = false;
}

void
WallpaperViewWidget::dropImage ()
{
    /*
     * Notifying the business logic about the editing ended. It is important,
     * otherwise the businesslogic will reject the next edit start requests.
     */
    m_BusinessLogic->endEdit ();    
}

void
WallpaperViewWidget::initialize (
        QuillImage   &image,
        QSize         size)
{
    WallpaperDescriptor  desc = m_BusinessLogic->editedImage ();
    QSize                sceneSize;
    int                  xMarg = 0;
    int                  yMarg = 0;

    if (m_Initialized) {
        SYS_WARNING ("Already initialized.");
        return;
    }

    if (desc.isNull()) {
        SYS_WARNING ("No editted image, giving up.");
        return;
    }
    
    m_Image        = image;
    m_OriginalSize = size;
    sceneSize = m_BusinessLogic->sceneSize ();
    m_Trans.setExpectedSize (sceneSize);
    m_Trans.setOrientation (M::Portrait);
    
    /*
     * The margins can be positive or negative, so that the image is always
     * centered at first.
     */
    xMarg = (sceneSize.width() - m_Image.width()) / 2;
    yMarg = (sceneSize.height() - m_Image.height()) / 2;

    m_Trans.setOffset (QPointF(xMarg, yMarg));
    this->setMinimumSize (m_Trans.expectedSize());
    
    m_ImageWidget = new MImageWidget(&m_Image, this);
    
    setupPanningPhysics (true);
    m_Initialized = true;
}

bool 
WallpaperViewWidget::pagePans () const
{
    return false;
}


void
WallpaperViewWidget::polishEvent ()
{
    QGraphicsWidget  *parent;
    MApplicationPage *page = 0;

    /*
     * Just a protection about double adding the actions.
     */
    if (m_PageRealized)
        return;
    
    /*
     * We need to find the MApplicationPage among our parents.
     */
    parent = parentWidget();
    while (parent) {
        page = qobject_cast <MApplicationPage *>(parent);
        if (page)
            break;
        parent = parent->parentWidget();
    }
    m_PageRealized = true;

    /*
     * With or without the page we can turn the window fullscreen.
     */
    if (Wallpaper::useFullScreen) {
        MWindow *win = MApplication::activeWindow ();

        if (win)
            win->showFullScreen();
    }

    if (!page)
        return;

    page->setPannable (false);
    page->pannableViewport()->positionIndicator()->hide();
    /**************************************************************************
     * Hiding the home button and the escape button from the page. 
     */
    page->setComponentsDisplayMode (
            MApplicationPage::EscapeButton,
            MApplicationPageModel::Hide);
    page->setComponentsDisplayMode (
            MApplicationPage::HomeButton,
            MApplicationPageModel::Hide);

    //% "Save"
    m_DoneAction = new MAction(qtTrId("qtn_comm_save"), this);
    m_DoneAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_DoneAction);
    connect(m_DoneAction, SIGNAL(triggered()), this, SLOT(slotDoneActivated()));

    //% "Cancel"
    m_CancelAction = new MAction(qtTrId("qtn_comm_cancel"), this);
    m_CancelAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_CancelAction);
    connect(m_CancelAction, SIGNAL(triggered()), this, SLOT(slotCancelActivated()));
}

/*!
 * Re-implemented method for setting back the non-fullscreen window-state
 */
bool 
WallpaperViewWidget::back ()
{
    MWindow *win;

    /*
     * Turning back from fullscreen. This could be done in the destructor, but
     * that ends up with a segfault in the Qt...
     */

    win = MApplication::activeWindow ();
    if (win)
        win->showNormal();

    return DcpStylableWidget::back();
}



int
WallpaperViewWidget::imageX () const
{
    int retval = 0;

    //retval += m_Trans.x();
    retval += m_UserOffset.x();

    return retval;
}

int
WallpaperViewWidget::imageY () const
{
    int retval = 0;

    //retval += m_Trans.y();
    retval += m_UserOffset.y();

    return retval;
}

int
WallpaperViewWidget::imageDX () const
{
    return m_Trans.scale() * m_Image.width();
}

int
WallpaperViewWidget::imageDY () const
{
    return m_Trans.scale() * m_Image.height();
}

QPixmap
WallpaperViewWidget::generatePixmap (
        const WallpaperITrans  &transformations)
{
    QImage    transformed = transformedImage();
    QPixmap   retval (transformations.expectedSize());
    QPainter  painter (&retval);
    QPointF   offset (
            imageX() - transformed.width() / 2, 
            imageY() - transformed.height() / 2);

    retval.fill (m_BgColor);
    painter.drawImage (offset, transformed);

    return retval;
}

    
void 
WallpaperViewWidget::slotDoneActivated ()
{
    if (m_Saving) {
        SYS_DEBUG ("Saving is being performed.");
        return;
    }

    /*
     * We ignore the button press while the image is moving.
     */
    if (m_Physics->inMotion())
        m_Physics->stop();

    saveImage ();
}
 
void 
WallpaperViewWidget::slotCancelActivated ()
{
    if (m_Saving) {
        SYS_DEBUG ("Saving is being performed.");
        return;
    }

    /*
     * We ignore the button press while the image is moving.
     */
    if (m_Physics->inMotion())
        m_Physics->stop();

    /*
     * Emitting a signal to show the cancel is clicked. Then emitting a signal
     * to close the application page (this slot is only called when the sheet is
     * not used). Then we notify the business logic about the edit operation is
     * finished.
     */
    emit cancelClicked ();
    emit closePage ();
    m_BusinessLogic->endEdit ();
}

void 
WallpaperViewWidget::wallpaperSaved ()
{
    if (!m_Saving) {
        SYS_WARNING ("We didn't request any saving.");
        return;
    }

    /*
     * Turning back from fullscreen. This could be done in the destructor, but
     * that ends up with a segfault in the Qt...
     */
    MWindow  *win;
    win = MApplication::activeWindow ();
    if (win)
        win->showNormal();

    if (!Wallpaper::useSheets) {
        /*
         * We are done with the editing, let's page back to the view where we
         * have the list.
         */
        //changeWidget (0);
        emit closePage ();
    }
    
    m_BusinessLogic->endEdit ();
    m_Saving = false;
}


void 
WallpaperViewWidget::wallpaperLoaded (
        QuillImage    image, 
        QSize         originalSize)
{
    if (!m_Initialized) {
        initialize (image, originalSize);
    } else {
        SYS_WARNING ("Progressive loading not implemented.");
    }
}

/******************************************************************************
 * FIXME: Do we need frame-dropping?
 */
void 
WallpaperViewWidget::queueRedrawImage ()
{
    if (m_HasPendingRedraw) {
        return;
    }

    m_HasPendingRedraw = true;
    QTimer::singleShot(50, this, SLOT(redrawImage ()));
}

void 
WallpaperViewWidget::redrawImage ()
{
    /*
     * There was some strange transient since we use full screen sheets, this is
     * how we fixed it.
     */
    if (m_ImageWidget->size().width() == 0 ||
            m_ImageWidget->size().height() == 0)
        return;

    QSize   imageSize = imageVisualSize ();
    QPointF offset (
            imageX() - m_ImageWidget->size().width() / 2.0, 
            imageY() - m_ImageWidget->size().height() / 2.0);

    m_ImageWidget->setTransformOriginPoint(
            m_ImageWidget->size().width() / 2.0, 
            m_ImageWidget->size().height() / 2.0);

    m_ImageWidget->setScale (m_Trans.scale());
    
    m_ImageWidget->setPos (offset);
    m_ImageWidget->setRotation (m_Trans.rotation());
    m_HasPendingRedraw = false;
}

QSize
WallpaperViewWidget::imageVisualSize (qreal scale)
{
    qreal realdx, realdy;
    QSize  retval;

    if (m_Trans.rotation() == 90.0 || m_Trans.rotation() == -90.0) {
        realdx = m_Image.height(); 
        realdy = m_Image.width(); 
    } else {
        realdy = m_Image.height();
        realdx = m_Image.width();
    }

    retval = QSize (realdx * scale, realdy * scale);
    return retval;
}

QImage
WallpaperViewWidget::transformedImage ()
{
    QSize  visualSize (imageVisualSize(m_Trans.scale()));
    QImage retval (visualSize, QImage::Format_RGB16);
    QPainter  painter (&retval);
    QRectF rect;

    retval.fill (QColor("#ffffff").rgb());
    painter.translate(
            retval.width() / 2, 
            retval.height() / 2); 
    painter.rotate(m_Trans.rotation());
   
    if (m_Trans.rotation() == 90.0 || m_Trans.rotation() == -90.0)
        rect = QRectF (
                -retval.height() / 2, 
                -retval.width() / 2, 
                visualSize.height(),
                visualSize.width());
    else
        rect = QRectF (
                -retval.width() / 2, 
                -retval.height() / 2, 
                visualSize.width(),
                visualSize.height());


    painter.drawImage (rect, m_Image);
    return retval;
}

/******************************************************************************
 * Stuff for the normal mouse events.
 */
void 
WallpaperViewWidget::wheelEvent (
        QGraphicsSceneWheelEvent *event)
{
    Q_UNUSED (event);

    bool     ctrl = QApplication::keyboardModifiers() & Qt::ControlModifier;

    Q_UNUSED (ctrl);
   
    if (!m_SupportPinchEdit)
        return;
    /*
     * We might auto-rotate. If we do we don't accept nothing.
     */
    if (m_RotateAnimation.state() == QAbstractAnimation::Running)
        return;


    m_Physics->stop ();
#ifdef XYZ
    m_ScalePhysics->pointerPress(QPointF());
    
    if (!ctrl) {
        m_Rotating = true;
        m_ScalePhysics->pointerMove (QPointF(0.0,  event->delta() / 100.0));
    } else {
        m_Scaling = true;
        m_ScalePhysics->pointerMove (QPointF(event->delta() / 100.0, 0.0));
    }

    m_ScalePhysics->pointerRelease();
#endif

    m_Rotating = false;
    m_Scaling = false;
}


void
WallpaperViewWidget::mousePressEvent (
        QGraphicsSceneMouseEvent *event)
{
    SYS_WARNING ("");
    Q_UNUSED (event);
    m_Physics->stop();

    m_FullScreen = !m_FullScreen;
    emit fullScreenRequest (m_FullScreen);
}

/*******************************************************************************
 * Stuff for the one finger gestures.
 */
void 
WallpaperViewWidget::panGestureEvent (
        QGestureEvent *event, 
        QPanGesture   *panGesture)
{
    QTransform itemTransform(sceneTransform().inverted());
    QPointF itemSpaceOffset = 
        panGesture->offset() * itemTransform - 
        QPointF(itemTransform.dx(),itemTransform.dy());

    if (m_PinchOngoing || m_Saving)
        return;
    
    if (!m_SupportPanEdit)
        goto finalize;

    switch (panGesture->state()) {
        case Qt::GestureStarted:
            SYS_DEBUG ("Qt::GestureStarted");
            m_PanOngoing = true;
            m_Physics->pointerPress(QPointF());
            m_Physics->pointerMove(-itemSpaceOffset);
            break;

        case Qt::GestureUpdated:
            SYS_DEBUG ("Qt::GestureUpdated");
            SYS_DEBUG ("itemSpaceOffset = %s", SYS_POINTF(itemSpaceOffset));
            m_Physics->pointerMove(-itemSpaceOffset);
            break;

        case Qt::GestureFinished:
            SYS_DEBUG ("Qt::GestureFinished");
            m_Physics->pointerRelease();
            m_PanOngoing = false;
            break;

        case Qt::GestureCanceled:
            SYS_DEBUG ("Qt::GestureCanceled");
            m_Physics->pointerRelease();
            m_PanOngoing = false;
            break;
        
        case Qt::NoGesture:
            SYS_DEBUG ("Qt::NoGesture");
            SYS_WARNING ("I dont know what to do when Qt::NoGesture");
            break;
    }

finalize:
    event->accept (panGesture);
}

/*******************************************************************************
 * Stuff for the two finger gestures.
 */
void 
WallpaperViewWidget::pinchGestureEvent (
            QGestureEvent *event, 
            QPinchGesture *pinchGesture)
{
    Q_UNUSED (event);

    if (!m_SupportPinchEdit)
        goto finalize;

    if (m_Saving)
        return;

    if (m_PanOngoing) {
        SYS_WARNING ("Aborting panning?");
        m_Physics->pointerRelease();
        m_PanOngoing = false;
        return;
    }

    switch (pinchGesture->state()) {
        case Qt::GestureStarted:
            SYS_DEBUG ("Qt::GestureStarted");
            m_PinchOngoing = true;
            pinchGestureStarted (event, pinchGesture);
            break;

        case Qt::GestureFinished:
            SYS_DEBUG ("Qt::GestureFinished");
            pinchGestureEnded (event, pinchGesture);
            m_PinchOngoing = false;
            break;

        case Qt::GestureCanceled:
            SYS_DEBUG ("Qt::GestureCanceled");
            pinchGestureEnded (event, pinchGesture);
            m_PinchOngoing = false;
            break;

        case Qt::GestureUpdated:
            SYS_DEBUG ("Qt::GestureUpdated");
            pinchGestureUpdate (event, pinchGesture);
            break;

        case Qt::NoGesture:
            SYS_DEBUG ("Qt::NoGesture");
            SYS_WARNING ("I dont know what to do when Qt::NoGesture");
            break;
    }

finalize:
    event->accept (pinchGesture);
}

void 
WallpaperViewWidget::pinchGestureStarted (
        QGestureEvent *event, 
        QPinchGesture *pinchGesture)
{
    Q_UNUSED (event);
    Q_UNUSED (pinchGesture);
    /*
     * We might auto-rotate. If we do we don't accept nothing.
     */
    if (m_RotateAnimation.state() == QAbstractAnimation::Running)
        return;
    
    /*
     * Save some values for pinchGestureUpdate
     */
    m_LastTransScale     = m_Trans.scale ();
    m_LastTransRotation  = m_Trans.rotation ();
}

void 
WallpaperViewWidget::pinchGestureUpdate (
            QGestureEvent *event, 
            QPinchGesture *pinchGesture)
{
    /*
     * 
     */
    Q_UNUSED (event);

    if (!m_Scaling && !m_Rotating) {
        qreal scaleDiff = pinchGesture->totalScaleFactor();
        qreal rotationDiff = pinchGesture->rotationAngle();

        if (qAbs (rotationDiff) > 10.0) {
            m_Rotating = true;
        } else if (scaleDiff > 1.1 || scaleDiff < 0.9) {
            m_Scaling = true;
        }
    }

    if (m_Rotating) {
        // Keep it very simple:
        m_Trans.setRotation (pinchGesture->rotationAngle () + m_LastTransRotation);
    } else if (m_Scaling) {
        /*
         * TODO: friction ... etc
         */
        qreal newScale = pinchGesture->totalScaleFactor () * m_LastTransScale;

        /* Limiting */
        if (newScale < scaleLowerLimit)
            newScale = scaleLowerLimit;
        else if (newScale > scaleUpperLimit)
            newScale = scaleUpperLimit;

        m_Trans.setScale (newScale);
    }

    queueRedrawImage ();
}

void 
WallpaperViewWidget::pinchGestureEnded (
            QGestureEvent *event, 
            QPinchGesture *pinchGesture)
{
    Q_UNUSED (event);
    Q_UNUSED (pinchGesture);

    pinchStopped ();

    m_Scaling = false;
    m_Rotating = false;
}

void 
WallpaperViewWidget::setupPanningPhysics (
        bool movePh)
{
    /*
     * The widget's geometry is disturbed by the transparent toolbar, 
     * but the expectedsize is stable.
     */
    QSize   geom = m_Trans.expectedSize();
    QSize   imageSize;
    qreal   left, top;
    qreal   width, height;
    int     imagedx, imagedy;

    imageSize = imageVisualSize (m_Trans.scale());

    if (m_Trans.rotation() == 90.0 || m_Trans.rotation() == -90.0) {
        imagedx = imageDY();
        imagedy = imageDX();
    } else {
        imagedx = imageDX();
        imagedy = imageDY();
    }
    
    if (geom.height() >= imagedy) {
        top    = imageSize.height() / 2.0;
        height = geom.height() - imageSize.height();
    } else {
        top     = geom.height() - imageSize.height() / 2.0;
        height  =  imageSize.height() - geom.height();
    }

    if (geom.width() >= imagedx) {
        left    = imageSize.width() / 2.0;
        width = geom.width() - imageSize.width();
    } else {
        left    = geom.width() - imageSize.width() / 2.0;
        width   =  imageSize.width() - geom.width();
    }
   
    #if 0
    SYS_WARNING ("*** imageSize = %s", SYS_SIZE(imageSize));
    SYS_WARNING ("*** offset  = %g, %g",
            m_Trans.offset().x(), m_Trans.offset().y());
    SYS_WARNING ("*** imaged. = %dx%d", imagedx, imagedy);
    SYS_WARNING ("*** range   = %g, %g (%gx%g)", left, top, width, height);
    #endif

    if (m_Physics)
        m_Physics->setRange (QRectF(left, top, width, height));
   
    /*
     *
     */
    if (m_Physics && movePh) {
        qreal hMargin;
        qreal vMargin;

        if (width == 0.0) {
            hMargin = left;
        } else {
            hMargin = left + (width - left) / 2.0;
        }

        if (height == 0.0) {
            vMargin = top;
        } else {
            vMargin = top + (height - top) / 2.0;
        }

        m_Physics->setPosition (QPointF(hMargin, vMargin));
    }
}

