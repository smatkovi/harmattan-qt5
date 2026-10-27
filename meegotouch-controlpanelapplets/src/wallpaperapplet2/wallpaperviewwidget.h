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
#ifndef WALLPAPERVIEWWIDGET_H
#define WALLPAPERVIEWWIDGET_H

#include "wallpaperviewwidgetstyle.h"
#include "wallpaperbusinesslogic.h"
#include "wallpaperdescriptor.h"
#include "wallpaperitrans.h"

#include <DcpStylableWidget>
#include <QPointer>
#include <MAction>
#include <MImageWidget>
#include <MPhysics2DPanning>
#include <QPropertyAnimation>

class WallpaperViewWidget : public DcpStylableWidget
{
    Q_OBJECT
    
    public:
        WallpaperViewWidget (
                WallpaperBusinessLogic *wallpaperBusinessLogic, 
                QGraphicsWidget        *parent = 0);
        ~WallpaperViewWidget ();

        virtual void saveImage ();
        virtual void dropImage ();

        virtual bool pagePans () const; 

        virtual bool back ();

    signals:
        void doneClicked ();
        void cancelClicked ();
        void fullScreenRequest (bool fullScreen);

    protected:
        virtual void applyStyle ();
        virtual void polishEvent ();
        virtual int imageX () const;
        virtual int imageY () const;
        virtual int imageDX () const;
        virtual int imageDY () const;
        virtual QPixmap generatePixmap (
                const WallpaperITrans  &transformations);
        QSize imageVisualSize (qreal scale = 1.0);
        
        /*
         * Normal mouse events.
         */
        virtual void mousePressEvent (QGraphicsSceneMouseEvent *event);
        virtual void wheelEvent (QGraphicsSceneWheelEvent *event);

        /*
         * One finger gestures.
         */
        void panGestureEvent (
                QGestureEvent *event, 
                QPanGesture   *panGesture);
        /*
         * Two finger gestures.
         */
        virtual void pinchGestureEvent (
                QGestureEvent *event, 
                QPinchGesture *pinchGesture);

        virtual void pinchGestureStarted (
                QGestureEvent *event, 
                QPinchGesture *pinchGesture);

        virtual void pinchGestureUpdate (
                QGestureEvent *event, 
                QPinchGesture *pinchGesture);
    
        virtual void pinchGestureEnded (
                QGestureEvent *event, 
                QPinchGesture *pinchGesture);        


    protected slots:
        virtual void initialize (QuillImage &image, QSize size);
        virtual void slotDoneActivated ();
        virtual void slotCancelActivated ();
        virtual void wallpaperSaved ();
        virtual void wallpaperLoaded (QuillImage image, QSize originalSize);
        void queueRedrawImage ();
        virtual void redrawImage ();

        /*
         * Handling physics as they are changing.
         */
        void panningPhysicsPositionChanged (const QPointF &position);
        void panningPhysicsPanningStopped ();
        //void pinchUpdate ();
        void pinchStopped ();
        void rotateAnimationFinished ();

    protected:
        QPointer<WallpaperBusinessLogic>  m_BusinessLogic;
        WallpaperITrans                   m_Trans;
        QColor                            m_BgColor;
        QuillImage                        m_Image;
        QSize                             m_OriginalSize;
        bool                              m_Saving;
        bool                              m_Initialized;
        MImageWidget                     *m_ImageWidget;

    private:
        M_STYLABLE_WIDGET(WallpaperViewWidgetStyle)
        QImage transformedImage ();
        void setupPanningPhysics (bool movePh = false);

    private:
        /*
         * Tag variables for the editor functionality.
         */
        bool                              m_PageRealized;
        QPointer<MAction>                 m_DoneAction;
        QPointer<MAction>                 m_CancelAction;

        private:
            bool                  m_OrientationLocked;
            M::Orientation        m_Orientation;

            QPointF               m_UserOffset;

            qreal                 m_LastTransScale;
            qreal                 m_LastTransRotation;
            bool                  m_PinchOngoing;
            bool                  m_Scaling;
            bool                  m_Rotating;
            bool                  m_PanOngoing;
            bool                  m_HasPendingRedraw;
            bool                  m_HasPendingSave;
            MPhysics2DPanning    *m_Physics;
            QPropertyAnimation    m_RotateAnimation;
            bool                  m_SupportPanEdit;
            bool                  m_SupportPinchEdit;
            bool                  m_FullScreen;

        friend class Ut_WallpaperEditorWidget;
};

#endif
