/***************************************************************************
**
** Changes to a page of the home screen, done once for a burst of them and
** while the home's window is to be seen only (see deferredupdate.h).
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "deferredupdate.h"
#include "mainwindow.h"

// how long a burst may pause and still be one
static const int BURST_MS = 50;

DeferredUpdate::DeferredUpdate(QObject *parent, const char *slot) :
    QObject(parent),
    pending(false)
{
    timer.setSingleShot(true);
    timer.setInterval(BURST_MS);
    connect(&timer, SIGNAL(timeout()), SLOT(run()));
    connect(this, SIGNAL(update()), parent, slot);
    connect(MainWindow::instance(), SIGNAL(displayEntered()), SLOT(windowEntered()));
}

void DeferredUpdate::request()
{
    pending = true;
    if (MainWindow::instance()->isOnDisplay())
        timer.start();
}

void DeferredUpdate::run()
{
    if (!pending)
        return;
    if (!MainWindow::instance()->isOnDisplay())
        // when it comes back
        return;
    pending = false;
    emit update();
}

void DeferredUpdate::windowEntered()
{
    if (pending)
        run();
}
