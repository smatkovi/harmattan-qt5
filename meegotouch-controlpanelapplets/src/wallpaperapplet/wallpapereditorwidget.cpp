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
#include "wallpapereditorwidget.h"
#include "wallpapercurrentdescriptor.h"
#include "wallpaperinfoheader.h"

#include <QStyle>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QPixmap>
#include <QImage>
#include <QTimer>
#include <MPannableViewport>
#include <MPositionIndicator>

#ifndef NO_EDITING
#include <QGestureEvent>
#include <QTapAndHoldGesture>
#include <QPanGesture>
#include <QPinchGesture>
#include <QTapGesture>
#include <QSwipeGesture>
#endif

#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MAction>
#include <MApplicationWindow>
#include <MApplication>
#include <MApplicationPage>

#include "mwidgetcreator.h"
M_REGISTER_WIDGET_NO_CREATE(WallpaperEditorWidget)

static const qreal ScaleLowerLimit = 0.15;

//#define DEBUG
//#define WARNING
#include "../debug.h"

static const int ExtraMargin = 0;
static const int TitleBarHeight = 60;
static const int alwaysVisibleFromImage = 15;
static const qreal scaleLowerLimit = 20.0;
static const qreal scaleUpperLimit = 400.0;

/*!
 * WallpaperEditorWidget constructor
 *
 * \param wallpaperBusinessLogic An WallpaperBusinessLogic instance
 * \param parent The parent widget (if any)
 */
WallpaperEditorWidget::WallpaperEditorWidget (
        WallpaperBusinessLogic *wallpaperBusinessLogic, 
        QGraphicsWidget        *parent) :
    DcpWidget (parent),
    m_WallpaperBusinessLogic (wallpaperBusinessLogic),
    m_InfoHeader (0),
    m_DoneAction (0),
    m_CancelAction (0),
    m_NoTitlebar (false),
    m_OrientationLocked (false),
    m_PinchOngoing (false),
    m_PanOngoing (false),
    m_HasPendingRedraw (false),
    m_Physics (0)
{
    MWindow *win = MApplication::activeWindow ();
    
#ifndef LIBMEEGOCONTROL
    /*
     * FIXME: With the new demo program this will not work. This must be a bug
     * in the libMeegoTouch!
     * NOTE: It also freezes the controlpanel sometimes, so I had to completely
     * remove.
     */
    if (win)
        win->showFullScreen();
#endif

#if 0
    /*
     * This one unfortunately will not turn to full-screen at all.
     */
    if (win) {
        Qt::WindowStates state = win->windowState();
        win->setWindowState(state | Qt::WindowFullScreen);
    }
#endif

    setObjectName ("WallpaperEditorWidget");
    /*
     * Creating the MPhysics2DPanning object.
     */
    m_Physics = new MPhysics2DPanning (this);
    m_Physics->setPanDirection (Qt::Vertical | Qt::Horizontal);
    m_Physics->setEnabled(true);
    
    m_Physics->setPointerSpringK(0.25);
    m_Physics->setFriction(0.6);
    m_Physics->setSlidingFriction(0.10);
    m_Physics->setBorderSpringK(0.02);
    m_Physics->setBorderFriction(0.15);
    m_Physics->setMaximumVelocity(70);

    connect (m_Physics, SIGNAL(positionChanged(const QPointF &)),
            this, SLOT(panningPhysicsPositionChanged(const QPointF &)));
    connect (m_Physics, SIGNAL(panningStopped()),
            this, SLOT(panningPhysicsPanningStopped()));

    /*
     *
     */
    m_ScalePhysics = new MPhysics2DPanning (this);
    m_ScalePhysics->setPanDirection (Qt::Vertical);
    m_ScalePhysics->setEnabled(true);
    
    m_ScalePhysics->setPointerSpringK(0.25);
    m_ScalePhysics->setFriction(0.6);
    m_ScalePhysics->setSlidingFriction(0.20);
    m_ScalePhysics->setBorderSpringK(0.02);
    m_ScalePhysics->setBorderFriction(0.25);
    m_ScalePhysics->setMaximumVelocity(70);
    m_ScalePhysics->setRange (
            QRectF(0.0, scaleLowerLimit, 0.0, scaleUpperLimit));
    m_ScalePhysics->setPosition (QPointF(0.0, 100.0));
    connect (m_ScalePhysics, SIGNAL(positionChanged(const QPointF &)),
            this, SLOT(scalePhysicsPositionChanged(const QPointF &)));
    /*
     *
     */
    QTimer::singleShot (0, this, SLOT(createContent()));

    if (win) {
        m_Orientation = win->orientation();
        m_OrientationLocked = win->isOrientationLocked ();
        connect (win, SIGNAL(orientationChanged(M::Orientation)),
                this, SLOT(orientationChanged(M::Orientation)));
    } else {
        SYS_WARNING ("We have a problem: We don't know the orientation! "
                    "Only in tests though...");
        m_Orientation = M::Landscape;
    }
}

/*!
 * Destructor for WallpaperEditorWidget
 */
WallpaperEditorWidget::~WallpaperEditorWidget ()
{
}

/*!
 * Paint method for WallpaperEditorWidget, used for 
 * drawing the edited image actual state
 */
void
WallpaperEditorWidget::paint (
        QPainter                          *painter,
        const QStyleOptionGraphicsItem    *option,
        QWidget                           *widget)
{
#if 0
    bool portrait = (geometry().height() > geometry().width());
#endif

    painter->fillRect (
            -ExtraMargin, -ExtraMargin, 
            m_Trans.expectedWidth (),
            m_Trans.expectedHeight (),
            QColor ("black"));
#if 0
    if (portrait) {
        painter->drawImage (
                QRect (imageX(), imageY(), imageDX(), imageDY()),
                m_bgPortrait);
    } else if (!portrait) {
        painter->drawImage (
                QRect (imageX(), imageY(), imageDX(), imageDY()),
                m_bgLandscape);
    }
#else
        painter->drawImage (
                QRect (imageX(), imageY(), imageDX(), imageDY()),
                m_bgPortrait);
#endif
    MWidget::paint (painter, option, widget);
}

/*!
 * Method for creating the editor-widget contents
 */
void
WallpaperEditorWidget::createContent ()
{
    MWindow             *win = MApplication::activeWindow ();
    WallpaperDescriptor *desc;

    SYS_DEBUG ("");

    desc = m_WallpaperBusinessLogic->editedImage();
    if (!desc) {
        SYS_WARNING ("There is no editedimage.");
        return;
    }

    if (desc->isCurrent()) {
        WallpaperCurrentDescriptor *cdesc;
       
        cdesc = qobject_cast<WallpaperCurrentDescriptor*> (desc);
        if (!cdesc) {
            SYS_WARNING ("This is the current wallpaper, but its type is"
                    "not <WallpaperCurrentDescriptor>");
            goto not_current_wallpaper;
        }

        m_LandscapeTrans = cdesc->iTrans (M::Landscape);
        m_PortraitTrans = cdesc->iTrans (M::Portrait);

        /*
         * FIXME: We are using the original pixmap here, but we could make this
         * be more generic, and use the original only when it differs from the
         * edited, then we could move it to the WallpaperDescriptor class...
         */
        if (supportsLandscape()) {
            QSize sceneSize = win->visibleSceneSize (M::Landscape);

            m_bgLandscape = cdesc->image (
                    WallpaperDescriptor::OriginalLandscape);

            m_LandscapeTrans.setExpectedSize (sceneSize);
            m_LandscapeTrans.setOrientation (M::Landscape);
        }

        if (supportsPortrait()) {
            QSize sceneSize = win->visibleSceneSize (M::Portrait);

            m_bgPortrait = cdesc->image (
                    WallpaperDescriptor::OriginalPortrait);

            m_PortraitTrans.setExpectedSize (sceneSize);
            m_PortraitTrans.setOrientation (M::Portrait);
            // Unfortunatelly this function has to be called from here
            // before we actually move the image to the right place
            setupPanningPhysics ();
            m_Physics->setPosition (m_PortraitTrans.offset());
            m_PortraitTrans.setOffset (QPointF(0, 0));
            m_ScalePhysics->setPosition (QPointF(0.0, 100.0 * m_PortraitTrans.scale()));
        }

        goto finalize;
    }

not_current_wallpaper:

    /*
     * Here is what we do when this is not the current image.  If the image is
     * very big the handling might be slow, so we scale it down.
     */
    if (supportsLandscape()) {
        QSize sceneSize = win->visibleSceneSize (M::Landscape);

        m_bgLandscape = desc->image (
                WallpaperDescriptor::Landscape);
            
        m_LandscapeTrans.setExpectedSize (sceneSize);
        m_LandscapeTrans.setOrientation (M::Landscape);
    }

    if (supportsPortrait()) {
        QSize sceneSize = win->visibleSceneSize (M::Portrait);
        int   xMarg = 0;
        int   yMarg = 0;

        m_bgPortrait = desc->image (WallpaperDescriptor::Portrait);
        m_PortraitTrans.setOrientation (M::Portrait);
        m_PortraitTrans.setExpectedSize (sceneSize);

        if (sceneSize.width() > m_bgPortrait.width())
            xMarg = (sceneSize.width() - m_bgPortrait.width()) / 2;
        if (sceneSize.height() > m_bgPortrait.height())
            yMarg = (sceneSize.height() - m_bgPortrait.height()) / 2;

        m_Physics->setPosition (QPointF(xMarg, yMarg));
    }


finalize:
    /*
     * Need to copy back.
     */
    m_Trans = m_Orientation == M::Portrait ? 
        m_PortraitTrans : m_LandscapeTrans;

    this->setMinimumSize (m_Trans.expectedSize());

    /*
     *
     */
    setupPanningPhysics ();

#ifndef NO_EDITING
    /*
     * Enabling two finger gestures.
     */
    grabGesture (Qt::PinchGesture, Qt::GestureFlags());
    grabGesture (Qt::PanGesture, Qt::GestureFlags());
#endif

    createWidgets ();
    redrawImage ();
}

/*!
 * Method for create wallpaper-editor internal widgets
 */
void
WallpaperEditorWidget::createWidgets ()
{
    SYS_DEBUG ("");
    setContentsMargins (0, 0, 0, 0);
    setWindowFrameMargins (0.0, 0.0, 0.0, 0.0);
    
    #ifdef USE_INFOHEADER
    MLayout               *layout;
    MLinearLayoutPolicy   *policy;
    /*
     *
     */
    layout = new MLayout;
    policy = new MLinearLayoutPolicy (layout, Qt::Vertical);
    layout->setContentsMargins (0, 0, 0, 0);

    /*
     * The information header.
     */
    SYS_DEBUG ("Creating WallpaperInfoHeader");
    m_InfoHeader = new WallpaperInfoHeader;
    m_InfoHeader->setObjectName ("WallpaperInfoHeader");
    policy->addItem (m_InfoHeader);

    policy->addStretch ();

    setLayout (layout);
    #endif

}

void 
WallpaperEditorWidget::scalePhysicsPositionChanged(
        const QPointF    &position)
{
    qreal scalefactor = position.y() / 100.0;

    if (scalefactor < 0.05)
        scalefactor = 0.05;

    SYS_DEBUG ("position    %g, %g", position.x(), position.y());
    SYS_DEBUG ("scalefactor %g", scalefactor);
    m_Trans.setScale (scalefactor);
    setupPanningPhysics ();
    queueRedrawImage ();
}

void 
WallpaperEditorWidget::panningPhysicsPositionChanged(
        const QPointF    &position)
{
    m_UserOffset = position;
    redrawImage ();
}

void 
WallpaperEditorWidget::panningPhysicsPanningStopped ()
{
    redrawImage ();
}

/*!
 * This virtual method is executed when we already have an MApplicationPage as
 * parent. We initialize the page here.
 */
void
WallpaperEditorWidget::polishEvent ()
{
    QGraphicsWidget  *parent;
    MApplicationPage *page = 0;

    /*
     * Just a protection about double adding the actions.
     */
    if (m_DoneAction)
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
    m_DoneAction = new MAction(qtTrId("qtn_comm_command_done"), this);
    m_DoneAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_DoneAction);
    connect(m_DoneAction, SIGNAL(triggered()), this, SLOT(slotDoneActivated()));

    //% "Cancel"
    m_CancelAction = new MAction(qtTrId("qtn_comm_cancel"), this);
    m_CancelAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_CancelAction);
    connect(m_CancelAction, SIGNAL(triggered()), this, SLOT(slotCancelActivated()));
}

void
WallpaperEditorWidget::saveImage ()
{
    WallpaperITrans   *ltrans, *ptrans;

    m_Trans += m_UserOffset;
    m_UserOffset = QPointF();
    

    ltrans = m_Trans.orientation() == M::Landscape ?
        &m_Trans : &m_LandscapeTrans;
    ptrans = m_Trans.orientation() == M::Portrait ?
        &m_Trans : &m_PortraitTrans;

    /*
     * Here we save the settings.
     */
    m_WallpaperBusinessLogic->setBackground (ltrans, ptrans);

    /*
     *
     */
    m_WallpaperBusinessLogic->setEditedImage (0);

}
void
WallpaperEditorWidget::dropImage ()
{
    /*
     *
     */
    m_WallpaperBusinessLogic->setEditedImage (0);
}

/*!
 * This slot is called when the user activates the 'done' action and so we have
 * to store the settings.
 */
void
WallpaperEditorWidget::slotDoneActivated ()
{
    MWindow  *win;

    /*
     * We ignore the button press while the image is moving.
     */
    if (m_Physics->inMotion())
        m_Physics->stop();

    if (m_ScalePhysics->inMotion()) 
        m_ScalePhysics->stop();

    saveImage ();

    /*
     * Turning back from fullscreen. This could be done in the destructor, but
     * that ends up with a segfault in the Qt...
     */
    win = MApplication::activeWindow ();
    if (win)
        win->showNormal();

    /*
     * We are done with the editing, let's page back to the view where we have
     * the list.
     */
    SYS_DEBUG ("Calling changeWidget()");
    emit doneClicked ();
    changeWidget (0);
}

/*!
 * A slot for handling the 'Cancel' button presses
 */
void
WallpaperEditorWidget::slotCancelActivated ()
{
    /*
     * We ignore the button press while the image is moving.
     */
    if (m_Physics->inMotion())
        m_Physics->stop();

    if (m_ScalePhysics->inMotion()) 
        m_ScalePhysics->stop();

    emit cancelClicked ();
    emit closePage ();
    /*
     *
     */
    m_WallpaperBusinessLogic->setEditedImage (0);
}

/*!
 * Re-implemented method for setting back the non-fullscreen window-state
 */
bool 
WallpaperEditorWidget::back ()
{
    MWindow *win;

    /*
     * Turning back from fullscreen. This could be done in the destructor, but
     * that ends up with a segfault in the Qt...
     */
    SYS_DEBUG ("");

    win = MApplication::activeWindow ();
    if (win)
        win->showNormal();

    return DcpWidget::back();
}

bool 
WallpaperEditorWidget::pagePans () const
{
    return false;
}

/******************************************************************************
 * Private low level functions.
 */

/*!
 * \returns The offset correction value for the new state.
 * \param show If the titlebar should be shown or not.
 *
 */
QPointF
WallpaperEditorWidget::toggleTitlebars (
        bool       show)
{
    MApplicationPage  *currentPage;
    #if 0
    /*
     * This mapToScene() is sensitive to the screen rotation.
     */
    QPointF r;
    r = mapToScene (0, 0);
    SYS_WARNING ("r = %g, %g", r.x(), r.y());
    #endif
    /*
     * If the requested state is the current state we don't need to hide/show
     * anything, we just return the correction value.
     */
    if (show == !m_NoTitlebar)
        goto finalize;

    /*
     * We do the showing/hiding here.
     */
    if (MApplication::activeApplicationWindow())
        currentPage = MApplication::activeApplicationWindow()->currentPage();
    else
        currentPage = 0;

    if (currentPage) {
        if (show) {
            SYS_DEBUG ("Showing titlebar");
            currentPage->setComponentsDisplayMode (
                    MApplicationPage::NavigationBar,
                    MApplicationPageModel::Show); 
            #ifdef USE_INFOHEADER
            m_InfoHeader->show ();
            #endif
        } else {
            SYS_DEBUG ("Hiding titlebar");
            currentPage->setComponentsDisplayMode (
                    MApplicationPage::NavigationBar, 
                    MApplicationPageModel::Hide);
            #ifdef USE_INFOHEADER
            m_InfoHeader->hide ();
            #endif
        }
    }

    m_NoTitlebar = !show;

finalize:
    /*
     * To return the correction value.
     */
    if (!show)
        return QPointF (0, TitleBarHeight);

    return QPointF (0, 0);
}

/*!
 * Returns the X offset where the image should be painted inside the widget. 
 */
int
WallpaperEditorWidget::imageX () const
{
    int retval = 0;

    retval -= ExtraMargin;
    retval += m_UserOffset.x();
    retval += m_Trans.x();

    return retval;
}

/*!
 * Returns the Y offset where the image should be painted inside the widget. 
 */
int
WallpaperEditorWidget::imageY () const
{
    int            retval = 0;
#if 0
    bool           portrait = (geometry().height() > geometry().width());
    QPointF        r;
    r = mapToScene (0.0, 0.0);

    SYS_WARNING ("*** portrait = %s", SYS_BOOL(portrait));
    SYS_WARNING ("*** origo    = %g, %g", r.x(), r.y());
    retval -= r.y(); 
#endif
    retval += m_UserOffset.y();
    retval += m_Trans.y();

    //SYS_DEBUG ("returning %d", retval);
    return retval;
}


int
WallpaperEditorWidget::imageDX () const
{
    bool portrait = (geometry().height() > geometry().width());

    return portrait ? 
        m_Trans * m_bgPortrait.width() : m_Trans * m_bgLandscape.width();
}

int
WallpaperEditorWidget::imageDY () const
{
    bool portrait = (geometry().height() > geometry().width());

    return portrait ? 
        m_Trans * m_bgPortrait.height() : m_Trans * m_bgLandscape.height();
}

void 
WallpaperEditorWidget::queueRedrawImage ()
{
    if (m_HasPendingRedraw) {
        //SYS_DEBUG ("Dropping...");
        return;
    }

    m_HasPendingRedraw = true;
    QTimer::singleShot(50, this, SLOT(redrawImage ()));
}

void 
WallpaperEditorWidget::redrawImage ()
{
    /*
     * We need to update the current page and not just this widget because of
     * those damn extra margins coming from somewhere.
     */
    //SYS_DEBUG ("Drawing...");
    m_HasPendingRedraw = false;
    if (MApplication::activeApplicationWindow())
        MApplication::activeApplicationWindow()->currentPage()->update();
}

void 
WallpaperEditorWidget::orientationChanged (
        M::Orientation orientation)
{
    if (m_Orientation == orientation) {
        SYS_WARNING ("This is the old orientation?!");
        return;
    }

    switch (orientation) {
        case M::Portrait:
            SYS_DEBUG ("Turned to portrait");
            m_LandscapeTrans = m_Trans;
            m_Trans = m_PortraitTrans;
            this->setMinimumSize (m_Trans.expectedSize());
            break;

        case M::Landscape:
            SYS_DEBUG ("Turned to landscape");
            m_PortraitTrans = m_Trans;
            m_Trans = m_LandscapeTrans;
            this->setMinimumSize (m_Trans.expectedSize());
            break;
    }
    
    m_Orientation = orientation;
    redrawImage ();
}

/******************************************************************************
 * Stuff for the normal mouse events.
 */
#ifndef NO_EDITING
void 
WallpaperEditorWidget::wheelEvent (
        QGraphicsSceneWheelEvent *event)
{
    m_Physics->stop ();
    m_ScalePhysics->pointerPress(QPointF());

    m_ScalePhysics->pointerMove (QPointF(0.0,  event->delta() / 100.0));
    m_ScalePhysics->pointerRelease();
}
#endif

void
WallpaperEditorWidget::retranslateUi()
{
    if (m_DoneAction)
        //% "Save"
        m_DoneAction->setText (qtTrId("qtn_comm_command_done"));

    if (m_CancelAction)
        //% "Cancel"
        m_CancelAction->setText (qtTrId("qtn_comm_cancel"));
}

#ifndef NO_EDITING
void
WallpaperEditorWidget::mousePressEvent (
        QGraphicsSceneMouseEvent *event)
{
    m_Physics->stop();
    //toggleTitlebars (false);
}
#endif

#ifndef NO_EDITING
/*******************************************************************************
 * Stuff for the two finger gestures.
 */
void 
WallpaperEditorWidget::panGestureEvent (
        QGestureEvent *event, 
        QPanGesture   *panGesture)
{
    QTransform itemTransform(sceneTransform().inverted());
    QPointF itemSpaceOffset = 
        panGesture->offset() * itemTransform - 
        QPointF(itemTransform.dx(),itemTransform.dy());

    if (m_PinchOngoing)
        return;

    switch (panGesture->state()) {
        case Qt::GestureStarted:
            m_PanOngoing = true;
            m_Physics->pointerPress(QPointF());
            m_Physics->pointerMove(-itemSpaceOffset);
            break;

        case Qt::GestureUpdated:
            m_Physics->pointerMove(-itemSpaceOffset);
        break;

        case Qt::GestureFinished:
            m_Physics->pointerRelease();
            m_PanOngoing = false;
            break;

        case Qt::GestureCanceled:
            m_Physics->pointerRelease();
            m_PanOngoing = false;
        break;
    }

    event->accept (panGesture);
}


void 
WallpaperEditorWidget::pinchGestureStarted (
        QGestureEvent *event, 
        QPinchGesture *gesture)
{
    if (m_PanOngoing) {
        m_Physics->pointerRelease();
        m_PanOngoing = false;
        return;
    }

    m_OriginalScaleFactor = m_Trans.scale();
    qreal startFrom = m_OriginalScaleFactor * 100.0;

    SYS_DEBUG ("m_ScalePhysics->pointerPress (0.0, %g)", startFrom);
    m_ScalePhysics->pointerPress(QPointF());
    event->accept(gesture);
}

void 
WallpaperEditorWidget::pinchGestureUpdate (
            QGestureEvent *event, 
            QPinchGesture *gesture)
{
    if (m_PanOngoing) {
        m_Physics->pointerRelease();
        m_PanOngoing = false;
        return;
    }
    /*
     * No frame drop here: the pinch gesture is much better this way...
     */
    qreal scaley =
        m_OriginalScaleFactor - 
        (gesture->totalScaleFactor() * m_OriginalScaleFactor);

    m_ScalePhysics->pointerMove(
            QPointF(0.0, scaley * 100.0));
    
    event->accept(gesture);
}

void 
WallpaperEditorWidget::pinchGestureEnded (
            QGestureEvent *event, 
            QPinchGesture *gesture)
{
    if (m_PanOngoing) {
        m_Physics->pointerRelease();
        m_PanOngoing = false;
        return;
    }

    m_ScalePhysics->pointerRelease ();
    event->accept(gesture);
}

void 
WallpaperEditorWidget::pinchGestureEvent (
            QGestureEvent *event, 
            QPinchGesture *gesture)
{
    Q_UNUSED (event);

    switch (gesture->state()) {
        case Qt::GestureStarted:
            m_PinchOngoing = true;
            pinchGestureStarted (event, gesture);
            break;

        case Qt::GestureFinished:
        case Qt::GestureCanceled:
            pinchGestureEnded (event, gesture);
            m_PinchOngoing = false;
            break;

        case Qt::GestureUpdated:
            pinchGestureUpdate (event, gesture);
    }
}

/*
 * The pinch gesture event coordinate system is not the same as the motion event
 * event coordinate system in portrait mode. This hackish workaround will
 * transform the coordinates if necessary.
 */
void 
WallpaperEditorWidget::gestureWorkaround (
        QPointF *point)
{
    bool portrait = (geometry().height() > geometry().width());

    if (portrait) {
        QPointF tmp (geometry().width() - point->y(), point->x());
        *point = tmp;
    }
}
#endif

bool
WallpaperEditorWidget::supportsLandscape () const
{
    return !m_OrientationLocked || m_Orientation == M::Landscape;
}

bool
WallpaperEditorWidget::supportsPortrait () const
{
    return !m_OrientationLocked || m_Orientation == M::Portrait;
}

void 
WallpaperEditorWidget::setupPanningPhysics ()
{
    /*
     * The widget's geometry is disturbed by the transparent toolbar, 
     * but the expectedsize is stable.
     */
    QSize   geom = m_Trans.expectedSize();
    qreal   left, top;
    qreal   width, height;
   

    if (geom.height() >= imageDY()) {
        top    = 0.0;
        height = geom.height() - imageDY();
    } else {
        top    = geom.height() - imageDY();
        height = -1.0 * top;
    }
    
    if (geom.width() >= imageDX()) {
        left    = 0.0;
        width   = geom.width() - imageDX();
    } else {
        left    = geom.width() - imageDX();
        width   = -1.0 * left;
    }

    m_Physics->setRange (QRectF(left, top, width, height));
}

