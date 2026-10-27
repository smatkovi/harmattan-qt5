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

#define DEBUG
#include "../../src/debug.h"


WallpaperViewWidget::WallpaperViewWidget (
        WallpaperBusinessLogic *wallpaperBusinessLogic, 
        QGraphicsWidget        *parent)
{
    Q_UNUSED (wallpaperBusinessLogic);
    Q_UNUSED (parent);
    SYS_DEBUG ("");
}

WallpaperViewWidget::~WallpaperViewWidget ()
{
    SYS_DEBUG ("");
}

void
WallpaperViewWidget::saveImage ()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::dropImage ()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::applyStyle()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::panningPhysicsPositionChanged(
        const QPointF    &position)
{
    Q_UNUSED (position);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::panningPhysicsPanningStopped ()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::pinchStopped ()
{
    SYS_DEBUG ("");
}

void
WallpaperViewWidget::rotateAnimationFinished ()
{
    SYS_DEBUG ("");
}

void
WallpaperViewWidget::initialize (
        QuillImage   &image,
        QSize         size)
{
    Q_UNUSED (image);
    Q_UNUSED (size);
    SYS_DEBUG ("");
}

bool 
WallpaperViewWidget::pagePans () const
{
    SYS_DEBUG ("");
    return false;
}

void
WallpaperViewWidget::polishEvent ()
{
    SYS_DEBUG ("");
}

bool 
WallpaperViewWidget::back ()
{
    SYS_DEBUG ("");
    return false;
}

int
WallpaperViewWidget::imageX () const
{
    SYS_DEBUG ("");
    return 0;
}

int
WallpaperViewWidget::imageY () const
{
    SYS_DEBUG ("");
    return 0;
}

int
WallpaperViewWidget::imageDX () const
{
    SYS_DEBUG ("");
    return 0;
}

int
WallpaperViewWidget::imageDY () const
{
    SYS_DEBUG ("");
    return 0;
}

QPixmap
WallpaperViewWidget::generatePixmap (
        const WallpaperITrans  &transformations)
{
    Q_UNUSED (transformations);
    SYS_DEBUG ("");
    return QPixmap();
}

void 
WallpaperViewWidget::slotDoneActivated ()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::slotCancelActivated ()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::wallpaperSaved ()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::wallpaperLoaded (
        QuillImage    image, 
        QSize         originalSize)
{
    Q_UNUSED (image);
    Q_UNUSED (originalSize);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::queueRedrawImage ()
{
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::redrawImage ()
{
    SYS_DEBUG ("");
}

QSize
WallpaperViewWidget::imageVisualSize (qreal scale)
{
    Q_UNUSED (scale);
    SYS_DEBUG ("");
    return QSize();
}

QImage
WallpaperViewWidget::transformedImage ()
{
    SYS_DEBUG ("");
    return QImage();
}

void 
WallpaperViewWidget::wheelEvent (
        QGraphicsSceneWheelEvent *event)
{
    Q_UNUSED (event);
    SYS_DEBUG ("");
}

void
WallpaperViewWidget::mousePressEvent (
        QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED (event);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::panGestureEvent (
        QGestureEvent *event, 
        QPanGesture   *panGesture)
{
    Q_UNUSED (event);
    Q_UNUSED (panGesture);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::pinchGestureEvent (
            QGestureEvent *event, 
            QPinchGesture *pinchGesture)
{
    Q_UNUSED (event);
    Q_UNUSED (pinchGesture);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::pinchGestureStarted (
        QGestureEvent *event, 
        QPinchGesture *pinchGesture)
{
    Q_UNUSED (event);
    Q_UNUSED (pinchGesture);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::pinchGestureUpdate (
            QGestureEvent *event, 
            QPinchGesture *pinchGesture)
{
    Q_UNUSED (event);
    Q_UNUSED (pinchGesture);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::pinchGestureEnded (
            QGestureEvent *event, 
            QPinchGesture *pinchGesture)
{
    Q_UNUSED (event);
    Q_UNUSED (pinchGesture);
    SYS_DEBUG ("");
}

void 
WallpaperViewWidget::setupPanningPhysics (
        bool movePh)
{
    Q_UNUSED (movePh);
    SYS_DEBUG ("");
}

