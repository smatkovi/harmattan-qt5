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
#include "ft_applets.h"

#include <QTest>
#include <QDir>
#include <QTimer>
#include <MTheme>
#include <MWidget>
#include <QGraphicsWidget>
#include <dcpappletif.h>
#include <dcpbrief.h>
#include <dcpwidget.h>
#include <dcpstylablewidget.h>
#include <MApplication>
#include <MApplicationWindow>

#define DEBUG
#define WARNING
#include "debug.h"

#define SYS_DEBUG_VIEW(type) \
            SYS_DEBUG ("have view: " TERM_YELLOW "%s" TERM_NORMAL, \
                type == Ft_Applets::NoView ? "No" : \
                  type == Ft_Applets::HaveView ? "Yes" : "Stylable");

/*
 * Stub QTimer singleShot to avoid random crashing due to
 * quick creation & destroy
 */
void
QTimer::singleShot(int msec, QObject *receiver, const char *member) {
    Q_UNUSED (receiver);
    Q_UNUSED (member);
    Q_UNUSED (msec);
    SYS_DEBUG (" ignoring singleshot [ %s, SLOT (%s) ] ",
               SYS_STR (receiver->objectName ()), member);
}

/*
 * To speed up testcases...
 */
const QPixmap *
MTheme::pixmap (const QString &id, const QSize &size)
{
    Q_UNUSED (id);
    QPixmap *ret = new QPixmap (size);
    return ret;
}

/*
 * Stub QGraphicsWidget::setLayout as it causes crash inside the
 * libmeegotouch sometimes :-O
 */
void
QGraphicsWidget::setLayout (QGraphicsLayout *layout)
{
    Q_UNUSED (layout);
}

/*
 * MWidget::event
 */
bool
MWidget::event (QEvent *event)
{
    Q_UNUSED (event);
    return false;
}

static int argc = 1;
static char *argv[] =
    {
        (char *) "./ft_applets",
        NULL
    };

void
Ft_Applets::initTestCase ()
{
    m_app = new MApplication (argc, argv);
    m_window = new MApplicationWindow;
}

void
Ft_Applets::cleanupTestCase ()
{
    delete m_window;
    m_window = 0;
    m_app->deleteLater ();
    m_app = 0;
}

void
Ft_Applets::init ()
{
}

void
Ft_Applets::cleanup ()
{
}

/*!
 * This function will do the basic loading/unloading tests on the applet.
 */
void
Ft_Applets::doAppletTest (const char *soname, bool hasBrief, view viewType)
{
    QPluginLoader loader;
    QDir appletPath (APPLET_LIBS);
    QString fileName = appletPath.absoluteFilePath (soname);
    SYS_DEBUG ("fileName = \"%s\"", SYS_STR (fileName));

    if (! QFile::exists (appletPath.absoluteFilePath (soname)))
    {
        SYS_WARNING ("Skipping this plugin as not installed.");
        return;
    }

    loader.setFileName (fileName);
    loader.setLoadHints (QLibrary::ResolveAllSymbolsHint);

    /*
     * Getting the DcpAppletIf applet interface object. Checking if the applet
     * loaded, the type of the applet interface is right.
     */
    QVERIFY2 (loader.load (), qPrintable (loader.fileName() +
                                          loader.errorString ()));


    DcpAppletIf *applet = qobject_cast<DcpAppletIf *>(loader.instance ());

    QVERIFY (applet != 0);

    /*
     * Do initialization... in the same way how controlpanel doing:
     */
    applet->init ();

    /*
     * Checking if the applet brief is constructed.
     */
    DcpBrief *brief = applet->constructBrief ();
    if (hasBrief)
    {
        QVERIFY2(brief, "Error when creating brief widget");
    }
    else
    {
        QVERIFY2(!brief, "This applet should not have a Brief");
    }

    SYS_DEBUG_VIEW(viewType);
    if (viewType > 0)
    {
        doViewTest (applet, viewType == HaveStylableView); 
    }

    delete brief;
    loader.unload ();
}

void
Ft_Applets::doViewTest (DcpAppletIf *applet, bool stylableView)
{
    QGraphicsWidget *widget = 0;

    QVERIFY (applet != 0);

    if (stylableView == false)
        widget = applet->constructWidget (0);
    else
        widget = applet->constructStylableWidget (0);

    QVERIFY2 (widget, "Error when creating applet's main widget!");

    delete widget;
}

void
Ft_Applets::testbatteryapplet ()
{
    doAppletTest ("libbatteryapplet.so", false, HaveStylableView);
}

void
Ft_Applets::testdisplayapplet ()
{
    doAppletTest ("libdisplayapplet.so", false);
}

void
Ft_Applets::testusbapplet ()
{
    doAppletTest ("libusbapplet.so", false, HaveStylableView);
}

void
Ft_Applets::testresetapplet ()
{
    doAppletTest ("libresetapplet.so");
}

void
Ft_Applets::testaboutapplet ()
{
#if 0
    /* FIXME: need to workaround the QThread
     * destroyed while it is running error....
     */
    doAppletTest ("libaboutapplet.so", false, HaveStylableView);
#endif
}

void
Ft_Applets::testthemeapplet ()
{
#if 0
    /* Not in use... */
    doAppletTest ("libthemeapplet.so");
#endif
}

void
Ft_Applets::testwallpaperapplet ()
{
    doAppletTest ("libwallpaperapplet2.so", false, HaveStylableView);
}

void
Ft_Applets::testwarrantyapplet ()
{
#if 0
    /* Not in use... */
    doAppletTest ("libwarrantyapplet.so", false, HaveStylableView);
#endif
}

void
Ft_Applets::testofflineapplet ()
{
    doAppletTest ("libofflineapplet.so", true, NoView);
}

void
Ft_Applets::testsoundsettingsapplet ()
{
    doAppletTest ("libsoundsettingsapplet.so", false, HaveStylableView);
}


QTEST_APPLESS_MAIN(Ft_Applets)

