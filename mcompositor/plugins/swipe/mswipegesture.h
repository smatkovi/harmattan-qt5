/***************************************************************************
**
** The N9's swipe: from a side edge of the screen the application is
** minimized (home and the open applications come up), from the top edge
** it is closed. The window follows the finger and what is below shows
** ("sneak peek"); let go before the threshold, it goes back.
**
** Written anew for the Qt 5 port of mcompositor (the N9's own swipe was
** not released).
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef MSWIPEGESTURE_H
#define MSWIPEGESTURE_H

// Qt's headers before Xlib's (its Status and Bool macros)
#include <QDataStream>
#include <QElapsedTimer>
#include <QObject>
#include <QTextStream>
#include <QtPlugin>
#include <QPointF>
#include <QSizeF>
#include <QPointer>
#include <QVariantAnimation>
#include <mcompositemanagerextension.h>
#include <mcompmgrextensionfactory.h>
#include <x11bool.h>

class MCompositeWindow;

class MSwipeGesture : public MCompositeManagerExtension
{
    Q_OBJECT

public:
    MSwipeGesture(QObject *parent = 0);
    ~MSwipeGesture();

protected:
    bool x11Event(XEvent *event);
    void afterX11Event(XEvent *) {}
    // mcompositor's own minimize and close animations are used
    bool providesAnimations() const { return false; }

private Q_SLOTS:
    void moveWindow(const QVariant &offset);
    void snapBackFinished();

private:
    enum Edge { NoEdge, LeftEdge, RightEdge, TopEdge };

    void grab();
    bool press(const XButtonEvent &e);
    void motion(const XMotionEvent &e);
    void release(const XButtonEvent &e);
    // the screen point in the application's own coordinates (its
    // orientation), and its size so
    QPointF logical(const QPointF &screen, QSizeF *size) const;
    QPointF screenOffset(const QPointF &logicalOffset) const;
    void setPeek(bool on);
    void closeCurrent();

    bool tracking;
    Edge edge;
    Window app;
    unsigned angle;
    QPointF start;          // logical
    QPointF offset;         // logical, how far the window has followed
    QSizeF logicalSize;
    QElapsedTimer pressTime;
    QVariantAnimation snapBack;
    QPointer<MCompositeWindow> window;

    // configuration (mcompositor's settings file)
    int edgeWidth;          // px from the edge where a swipe starts
    double threshold;       // part of the screen to go
    double flickThreshold;  // ... when quick
    int flickTime;          // ms that are quick
    bool closeFromTop;      // swipe down from the top closes
};

class MSwipeGestureFactory : public QObject, public MCompmgrExtensionFactory
{
    Q_OBJECT
    Q_INTERFACES(MCompmgrExtensionFactory)
    Q_PLUGIN_METADATA(IID "com.nokia.mCompositor.MCompmgrExtensionFactory/1.0")

public:
    MCompositeManagerExtension *create();
    QString extensionName();
};

#endif
