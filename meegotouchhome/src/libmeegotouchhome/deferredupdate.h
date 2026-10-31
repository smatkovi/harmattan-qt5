/***************************************************************************
**
** Changes to a page of the home screen, done once for a burst of them and
** while the home's window is to be seen only (written for the Qt 5 port).
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef DEFERREDUPDATE_H
#define DEFERREDUPDATE_H

#include <QObject>
#include <QTimer>

/*!
 * Calls a slot of its parent once for a burst of request()s (the system
 * UI hands over its notifications one by one, the launcher's data store
 * tells of each entry), shortly after the last; while the home's window
 * is not on display (an application in front of it, the display off),
 * not before it is again: a page that nobody sees is not built.
 */
class DeferredUpdate : public QObject
{
    Q_OBJECT

public:
    DeferredUpdate(QObject *parent, const char *slot);

public Q_SLOTS:
    void request();

private Q_SLOTS:
    void run();
    void windowEntered();

signals:
    void update();

private:
    QTimer timer;
    bool pending;
};

#endif
