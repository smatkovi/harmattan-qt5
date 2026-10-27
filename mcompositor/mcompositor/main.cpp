/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of mcompositor.
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

#include <QtGui>
#include <QtWidgets>
#include <QGLWidget>
#include "mcompositescene.h"
#include "mcompositemanager.h"

// @plugindir ?
#define PLUGINDIR "/usr/lib/mcompositor"

int main(int argc, char *argv[])
{
    // We don't need meego graphics system
    setenv("QT_GRAPHICSSYSTEM", "raster", 1);

#ifdef WINDOW_DEBUG
    // React to context-commander's fake events; required by test20.py
    // to be able to fake a phone call.
    setenv("CONTEXT_COMMANDING", "1", 1);
#endif
    
#if QT_VERSION < 0x050000
    // Don't load any Qt plugins
    QCoreApplication::setLibraryPaths(QStringList(PLUGINDIR));
#else
    // Qt 5 talks to X through plugins (the xcb platform and its GLX
    // integration): Qt's own plugins stay, and the compositor is X only
    setenv("QT_QPA_PLATFORM", "xcb", 1);
    QCoreApplication::setLibraryPaths(QStringList(PLUGINDIR)
        << QLibraryInfo::location(QLibraryInfo::PluginsPath));
#endif

    QApplication::setDesktopSettingsAware(false);
#if QT_VERSION < 0x050000
    // make QPixmap create X pixmaps
    QApplication::setGraphicsSystem("native");
#endif
    MCompositeManager app(argc, argv);

    QGraphicsScene *scene = app.scene();
    QGraphicsView view(scene);
    
    view.setFrameStyle(0);
    view.setProperty("NoMStyle", true);
#if QT_VERSION < 0x050000
    view.setUpdatesEnabled(false);
#endif
    // (in Qt 5 the viewport set below inherits the view's disabled
    // updates, and then glwidget->update() would never paint)
    view.setAutoFillBackground(false);
    view.setBackgroundBrush(Qt::NoBrush);
    view.setForegroundBrush(Qt::NoBrush);
    view.setFrameShadow(QFrame::Plain);

    view.setWindowFlags(Qt::X11BypassWindowManagerHint);
    view.setAttribute(Qt::WA_NoSystemBackground);
    view.setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    view.setOptimizationFlags(QGraphicsView::IndirectPainting);
    app.setSurfaceWindow(view.winId());

    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    view.setMinimumSize(QApplication::desktop()->width(),
                        QApplication::desktop()->height());
    view.setMaximumSize(QApplication::desktop()->width(),
                        QApplication::desktop()->height());

    QGLFormat fmt;
    fmt.setSamples(0);
    fmt.setSampleBuffers(false);

    QGLWidget *w = new QGLWidget(fmt);
#if QT_VERSION < 0x050000
    // (Qt 5 has no such attribute; a QGLWidget draws whenever its
    // context is current)
    w->setAttribute(Qt::WA_PaintOutsidePaintEvent);
#endif
    if (!app.isEgl()) {
        QPalette p = w->palette();
        p.setColor(QPalette::Background, QColor(Qt::black));
        w->setPalette(p);
        w->update();
    }
    w->setAutoFillBackground(false);
    w->setMinimumSize(QApplication::desktop()->width(),
                      QApplication::desktop()->height());
    w->setMaximumSize(QApplication::desktop()->width(),
                      QApplication::desktop()->height());
    app.setGLWidget(w);
    view.setViewport(w);
    w->makeCurrent();

    int testPlugin;
    const QStringList &args = app.arguments();
    for (testPlugin = 1; testPlugin < args.length(); testPlugin++)
        if (!args[testPlugin].isEmpty() && args[testPlugin][0] != '-')
            break;
    app.loadPlugins(testPlugin < args.length() ? args[testPlugin] : QString(),
                    PLUGINDIR);

    app.prepareEvents();
    app.redirectWindows();
    view.show();
    return app.exec();
}
