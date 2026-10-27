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

#include "ut_wallpapereditorwidget.h"
#include "wallpaperbusinesslogic.h"
#include "wallpaperviewwidget.h"
#include "wallpapergconf.h"

/*
 * We are reading data from this stub, we have some definition there we can use
 * in the test. This way the stub might be changed and these tests will
 * hopefully not fail.
 */
#include "mdesktopentry.h"

#include <MApplication>
#include <QList>
#include <QGraphicsSceneEvent>
#include <MGConfItem>

#define DEBUG
#include "../../src/debug.h"

#define UNKNOWN_BORDER_X 10
#define UNKNOWN_BORDER_Y 0

#define TITLEBAR_HEIGHT  60

#include <QPixmap>
bool
QPixmap::load (
        const QString &fileName,
        const char     *format,
        Qt::ImageConversionFlags flags)
{
    Q_UNUSED (fileName);
    Q_UNUSED (format);
    Q_UNUSED (flags);
    SYS_WARNING ("Emulating load of %s", SYS_STR(fileName));
    *this = QPixmap (864, 480);
    this->fill ();

    return true;
}

QPixmap
QPixmap::scaled (
        const QSize &size,
        Qt::AspectRatioMode aspectRatioMode,
        Qt::TransformationMode transformMode) const
{
    Q_UNUSED (aspectRatioMode);
    Q_UNUSED (transformMode);
    QPixmap retval (size);

    SYS_DEBUG ("Returning QPixmap of size %dx%d",
            size.width(), size.height());
    return retval;
}

#include <QImage>
bool
QImage::load (
        const QString &fileName,
        const char     *format)
{
    Q_UNUSED (fileName);
    Q_UNUSED (format);
    SYS_WARNING ("Emulating load of %s", SYS_STR(fileName));
    *this = QImage (QSize(864, 480), QImage::Format_RGB16);
    this->fill (0);

    return true;
}

/******************************************************************************
 * QGraphicsView stub.
 */
#include <QGraphicsItem>

/*
 * FIXME: Handling of this should be automatic, but we have no window, so we
 * have no page either.- Stubbing all of these would be too much pain in the
 * neck...
 */
static bool titlebarVisible = true;

QPointF
QGraphicsItem::mapToScene (
        const QPointF &point) const
{
    qreal   x, y;

    x = point.x ();
    y = point.y ();

    // The handling of this
    if (titlebarVisible)
        y += 60;

    SYS_DEBUG ("%g, %g >>-> %g, %g",
            point.x(), point.y(),
            x, y);

    return QPointF (x, y);
}

/******************************************************************************
 * MApplicationPage stub.
 *
 * We can't use this because there is no current window, so we don't have a page
 * either. Damn!
 */
#include <MApplicationPage>

void
MApplicationPage::setComponentsDisplayMode (
        Components components,
        MApplicationPageModel::ComponentDisplayMode displayMode)
{
    Q_UNUSED (components);
    SYS_WARNING ("--->");
    if (displayMode == MApplicationPageModel::Show) {
        SYS_DEBUG ("Emulating visible titlebar");
        titlebarVisible = true;
    } else if (displayMode == MApplicationPageModel::Hide) {
        SYS_DEBUG ("Emulating invisible titlebar");
        titlebarVisible = false;
    }
}

/******************************************************************************
 * MGConfItem stub.
 */
QVariant
MGConfItem::value () const
{
    QString retval;

    SYS_DEBUG ("*** key() = %s", SYS_STR(key()));

    if (key() == WALLPAPER_LANDSCAPE_KEY) {
        retval = WALLPAPER_LANDSCAPE_EDITEDFILE;
        goto return_string;
    }

    if (key() == WALLPAPER_PORTRAIT_KEY) {
        retval = WALLPAPER_PORTRAIT_EDITEDFILE;
        goto return_string;
    }

    return QVariant();

return_string:
    return QVariant (retval);
}

/******************************************************************************
 * Ut_WallpaperEditorWidget implementation.
 */
void
Ut_WallpaperEditorWidget::init()
{
}

void
Ut_WallpaperEditorWidget::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./ut_wallpapereditorwidget";

void
Ut_WallpaperEditorWidget::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_BusinessLogic = 0;
    m_Widget = 0;
}

void
Ut_WallpaperEditorWidget::cleanupTestCase()
{
    dropObjects ();
    m_App->deleteLater ();
}

/*
 * We crate the widget with no wallpaperdescriptor set to be under editing. In
 * this case the widget will not be able to do anything. In normal use this
 * scenario can not happen, but we need to test it anyway.
 */
void
Ut_WallpaperEditorWidget::testCreateContentFail ()
{
    createObjects ();
    QVERIFY (!m_Widget->m_DoneAction);
    QVERIFY (!m_Widget->m_CancelAction);
    QVERIFY (m_Widget->back());
    QVERIFY (!m_Widget->pagePans());
    dropObjects ();
}

void
Ut_WallpaperEditorWidget::test1 ()
{
    //m_BusinessLogic->m_EditedImage = WallpaperDescriptor("/editedimage.png");
    QuillImage image;
    QSize      size (1024, 1024);

    createObjects ();

    m_Widget->applyStyle ();
    m_Widget->initialize (image, size);
    
    dropObjects ();
}

void
Ut_WallpaperEditorWidget::test2 ()
{
    createObjects ();

    m_Widget->applyStyle ();
    QVERIFY (m_Widget->m_Physics);
    QVERIFY (m_Widget->m_Physics->panDirection () == 
            (Qt::Vertical | Qt::Horizontal));
    QVERIFY (m_Widget->m_Physics->enabled());
    
    m_Widget->applyStyle ();
    QVERIFY (m_Widget->m_Physics);
    QVERIFY (m_Widget->m_Physics->panDirection () == 
            (Qt::Vertical | Qt::Horizontal));
    QVERIFY (m_Widget->m_Physics->enabled());

    dropObjects ();
}

void
Ut_WallpaperEditorWidget::testCreateContentCurrent ()
{
#if 0
    /*
     * We crate the editor widget with the current wallpaper descriptor. The
     * data is coming from the mdesktopentry stub.
     */
    createObjects (DescriptorCurrent);

    /*
     * Checking the offsets here.
     */
    SYS_DEBUG ("imageX() = %d", m_Widget->imageX());
    SYS_DEBUG ("imageY() = %d", m_Widget->imageY());
    // Checking the offset if they are properly calculated. Please note that the
    // -10 is because of the unknown border (should be fixed) and the
    // -70 is because of the window titlebar height (should not be literal).
    QCOMPARE (m_Widget->imageX (),
              WALLPAPER_LANDSCAPE_HOROFFSET_NUM - UNKNOWN_BORDER_X);
    QCOMPARE  (m_Widget->imageY(),
              WALLPAPER_LANDSCAPE_VERTOFFSET_NUM - UNKNOWN_BORDER_Y
              /* - TITLEBAR_HEIGHT */);

    /*
     * We send the same orientation again. The editor widget should ignore this
     * one, it sets the orientation that is already set.
     */
    m_Widget->orientationChanged (M::Landscape);
    // The offsets sould remain the same.
    SYS_DEBUG ("imageX() = %d", m_Widget->imageX());
    SYS_DEBUG ("imageY() = %d", m_Widget->imageY());
    QCOMPARE (m_Widget->imageX(),
              WALLPAPER_LANDSCAPE_HOROFFSET_NUM - UNKNOWN_BORDER_X);
    QCOMPARE (m_Widget->imageY(),
              WALLPAPER_LANDSCAPE_VERTOFFSET_NUM - UNKNOWN_BORDER_Y
              /* - TITLEBAR_HEIGHT */);

    /*
     * Changing the orientation, checking if the offset is handled properly.
     */
    m_Widget->orientationChanged (M::Portrait);
    SYS_DEBUG ("imageX() = %d", m_Widget->imageX());
    SYS_DEBUG ("imageY() = %d", m_Widget->imageY());
    QCOMPARE (m_Widget->imageX(),
              WALLPAPER_PORTRAIT_HOROFFSET_NUM - UNKNOWN_BORDER_X);
    QCOMPARE (m_Widget->imageY(),
              WALLPAPER_PORTRAIT_VERTOFFSET_NUM - UNKNOWN_BORDER_Y
              /* - TITLEBAR_HEIGHT */);

    /*
     * Then turning to landscape again to test the change now that we are in
     * portrait mode.
     */
    m_Widget->orientationChanged (M::Landscape);
    // The offsets sould remain the same.
    SYS_DEBUG ("imageX() = %d", m_Widget->imageX());
    SYS_DEBUG ("imageY() = %d", m_Widget->imageY());
    QCOMPARE (m_Widget->imageX(),
              WALLPAPER_LANDSCAPE_HOROFFSET_NUM - UNKNOWN_BORDER_X);
    QCOMPARE (m_Widget->imageY(),
              WALLPAPER_LANDSCAPE_VERTOFFSET_NUM - UNKNOWN_BORDER_Y
              /* - TITLEBAR_HEIGHT */);

    /*
     * Destroying the widget.
     */
    dropObjects ();
#endif
}

void
Ut_WallpaperEditorWidget::testCreateContentNonCurrent ()
{
#if 0
    createObjects (DescriptorNotCurrent);
    /*
     * Checking the offsets. Please check the comments in
     * testCreateContentCurrent() for details.
     */
    SYS_DEBUG ("imageX() = %d", m_Widget->imageX());
    SYS_DEBUG ("imageY() = %d", m_Widget->imageY());
    QCOMPARE (m_Widget->imageX(),  - UNKNOWN_BORDER_X);
    QCOMPARE (m_Widget->imageY(), /* -TITLEBAR_HEIGHT */ - UNKNOWN_BORDER_Y);

    /*
     * Changing the orientation, checking if the offset is handled properly.
     */
    m_Widget->orientationChanged (M::Portrait);
    SYS_DEBUG ("imageX() = %d", m_Widget->imageX());
    SYS_DEBUG ("imageY() = %d", m_Widget->imageY());
    QCOMPARE (m_Widget->imageX(), - UNKNOWN_BORDER_X);
    QCOMPARE (m_Widget->imageY(), /* -TITLEBAR_HEIGHT  */ - UNKNOWN_BORDER_Y);

    dropObjects ();
#endif
}

void
Ut_WallpaperEditorWidget::testMouseEvents ()
{
#if 0
    QGraphicsSceneMouseEvent *pressEvent;
    QGraphicsSceneMouseEvent *moveEvent;
    QGraphicsSceneMouseEvent *releaseEvent;
    QPointF                   pressAt (23, 100);
    QPointF                   moveToRel (12, 13);
    QPointF                   moveTo (
            pressAt.x() + moveToRel.x(),
            pressAt.y() + moveToRel.y() + TITLEBAR_HEIGHT);

    /*
     * We use the current desciptor to test the mouse events.
     */
    createObjects (DescriptorCurrent);

    /*
     *
     */
    pressEvent = new QGraphicsSceneMouseEvent (QEvent::GraphicsSceneMousePress);
    pressEvent->setPos (pressAt);

    moveEvent = new QGraphicsSceneMouseEvent (QEvent::GraphicsSceneMouseMove);
    moveEvent->setPos (moveTo);

    releaseEvent = new QGraphicsSceneMouseEvent (
            QEvent::GraphicsSceneMouseRelease);

    /*
     * Sending the mouse press event. This should turn off the titlebar of the
     * window and modify the image offsets accordingly.
     */
    QCOMPARE (m_Widget->m_NoTitlebar, false);
    SYS_DEBUG ("*** imageY      = %d", m_Widget->imageY());
    SYS_DEBUG ("*** vertoffset  = %d", WALLPAPER_LANDSCAPE_VERTOFFSET_NUM);
    SYS_DEBUG ("*** unknownborder = %d", UNKNOWN_BORDER_Y);
//    SYS_DEBUG ("*** titlebar    = %d", TITLEBAR_HEIGHT);

    if (m_Widget->m_Orientation == M::Landscape) {
        QCOMPARE (m_Widget->imageX(),
                WALLPAPER_LANDSCAPE_HOROFFSET_NUM - UNKNOWN_BORDER_X);
        // fails
        QCOMPARE (m_Widget->imageY(),
                WALLPAPER_LANDSCAPE_VERTOFFSET_NUM - UNKNOWN_BORDER_Y);
               // - TITLEBAR_HEIGHT);
    } else {
        QCOMPARE (m_Widget->imageX(),
                WALLPAPER_PORTRAIT_HOROFFSET_NUM - UNKNOWN_BORDER_X);
        QCOMPARE (m_Widget->imageY(),
                WALLPAPER_PORTRAIT_VERTOFFSET_NUM - UNKNOWN_BORDER_Y);
//                - TITLEBAR_HEIGHT);
    }

    QCOMPARE (m_Widget->m_PinchOngoing, false);
    //
    // Sending the mouse press event here.
    //
    m_Widget->mousePressEvent (pressEvent);

    QCOMPARE (m_Widget->m_MotionOngoing, true);
    titlebarVisible = false;
    // Checking the changed.
    QCOMPARE (m_Widget->m_LastClick,
              QPointF (pressAt.x(), pressAt.y() + TITLEBAR_HEIGHT));
    QCOMPARE (m_Widget->m_NoTitlebar, true);
    if (m_Widget->m_Orientation == M::Landscape) {
        QCOMPARE (m_Widget->imageX(),
                WALLPAPER_LANDSCAPE_HOROFFSET_NUM - UNKNOWN_BORDER_X);
        QCOMPARE (m_Widget->imageY(),
                WALLPAPER_LANDSCAPE_VERTOFFSET_NUM - UNKNOWN_BORDER_Y);
    } else {
        QCOMPARE (m_Widget->imageX(),
                WALLPAPER_PORTRAIT_HOROFFSET_NUM - UNKNOWN_BORDER_X);
        QCOMPARE (m_Widget->imageY(),
                WALLPAPER_PORTRAIT_VERTOFFSET_NUM - UNKNOWN_BORDER_Y);
    }

    /*
     * Sending a mouse move event, dragging the image around the widget.
     */
    m_Widget->mouseMoveEvent (moveEvent);

    QCOMPARE (m_Widget->m_NoTitlebar, true);
    if (m_Widget->m_Orientation == M::Landscape) {
        QCOMPARE (m_Widget->imageX(),
                (int) (WALLPAPER_LANDSCAPE_HOROFFSET_NUM - UNKNOWN_BORDER_X +
                moveToRel.x()));
        QCOMPARE (m_Widget->imageY(),
                (int) (WALLPAPER_LANDSCAPE_VERTOFFSET_NUM - UNKNOWN_BORDER_Y +
                moveToRel.y()));
    } else {
        QCOMPARE (m_Widget->imageX(),
                (int) (WALLPAPER_PORTRAIT_HOROFFSET_NUM - UNKNOWN_BORDER_X +
                moveToRel.x()));
        QCOMPARE (m_Widget->imageY(),
                (int) (WALLPAPER_PORTRAIT_VERTOFFSET_NUM - UNKNOWN_BORDER_Y +
                moveToRel.y()));
    }

    /*
     * Sending the relase event. This should only change the image coordinates
     * in order to compensate the titlebar change; when the mouse button is
     * released the titlebar is shown again.
     */
    m_Widget->mouseReleaseEvent (releaseEvent);
    titlebarVisible = true;
    QCOMPARE (m_Widget->m_NoTitlebar, false);
    if (m_Widget->m_Orientation == M::Landscape) {
        QCOMPARE (m_Widget->imageX(),
                (int) (WALLPAPER_LANDSCAPE_HOROFFSET_NUM - UNKNOWN_BORDER_X +
                moveToRel.x()));
        QCOMPARE (m_Widget->imageY(),
                (int) (WALLPAPER_LANDSCAPE_VERTOFFSET_NUM - UNKNOWN_BORDER_Y
                /* - TITLEBAR_HEIGHT */ + moveToRel.y()));
    } else {
        QCOMPARE (m_Widget->imageX(),
                (int) (WALLPAPER_PORTRAIT_HOROFFSET_NUM - UNKNOWN_BORDER_X +
                moveToRel.x()));
        QCOMPARE (m_Widget->imageY(),
                (int) (WALLPAPER_PORTRAIT_VERTOFFSET_NUM - UNKNOWN_BORDER_Y
                /* - TITLEBAR_HEIGHT */ + moveToRel.y()));
    }

    /*
     * Cleaning up the resources.
     */
    delete pressEvent;
    delete moveEvent;
    delete releaseEvent;
    dropObjects ();
#endif
}

/******************************************************************************
 * Low level helper functions.
 */
void
Ut_WallpaperEditorWidget::createObjects (
       DescriptorType descType)
{
    QList<WallpaperDescriptor *> descriptors;

    m_BusinessLogic = new WallpaperBusinessLogic;
    switch (descType) {
        case DescriptorNone:
            // No image descriptor will be used.
            break;

        case DescriptorCurrent:
            // The current wallpaper will be edited.
            //m_BusinessLogic->setEditedImage (
            //        WallpaperCurrentDescriptor::instance());
            break;

        case DescriptorNotCurrent:
            // The not-current image will be edited. The index 0 contains the
            // current image descriptor.
            //descriptors = m_BusinessLogic->availableWallpapers ();
            //m_BusinessLogic->setEditedImage (descriptors[1]);
            break;
    }

    m_Widget = new WallpaperViewWidget (m_BusinessLogic);
    QTest::qWait (100);
}

void
Ut_WallpaperEditorWidget::dropObjects ()
{
    if (m_Widget)
        delete m_Widget;
    if (m_BusinessLogic)
        delete m_BusinessLogic;

    m_Widget = 0;
    m_BusinessLogic = 0;
}

QTEST_APPLESS_MAIN(Ut_WallpaperEditorWidget)



