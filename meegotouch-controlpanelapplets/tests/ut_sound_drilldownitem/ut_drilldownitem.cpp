/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
** Author:  David Kedves <dkedves@blumsoft.eu>
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
#include "ut_drilldownitem.h"
#include "drilldownitem.h"
#include <MApplication>
#include <QGraphicsLayout>

void
Ut_DrillDownItem::init ()
{
    m_raitem = 0;
}

void
Ut_DrillDownItem::cleanup ()
{
    delete m_raitem;
    m_raitem = 0;
}

static int argc = 1;
static char *app_name[] = { (char*) "./ut_sound_drilldownitem", NULL };

void
Ut_DrillDownItem::initTestCase ()
{
    m_App = new MApplication (argc, app_name);
}

void
Ut_DrillDownItem::cleanupTestCase ()
{
    delete m_App;
    m_App = 0;
}

void
Ut_DrillDownItem::testConstruct ()
{
    m_raitem = new RightArrowItem (MBasicListItem::TitleWithSubtitle, 0);
    QVERIFY (m_raitem);
}

void
Ut_DrillDownItem::testCreateLayout ()
{
    QGraphicsLayout *layout = 0;

    m_raitem = new RightArrowItem (MBasicListItem::TitleWithSubtitle, 0);
    layout = m_raitem->createLayout ();

    QVERIFY (layout != 0);

    delete m_raitem;
    layout = 0;

    m_raitem = new RightArrowItem (MBasicListItem::IconWithTitleAndSubtitle, 0);
    layout = m_raitem->createLayout ();

    QVERIFY (layout != 0);

    delete m_raitem;
    layout = 0;

    m_raitem = new RightArrowItem (MBasicListItem::IconWithTitle, 0);
    layout = m_raitem->createLayout ();

    QVERIFY (layout != 0);

    delete m_raitem;
    layout = 0;

    m_raitem = 0;
}

void
Ut_DrillDownItem::testTitleStyleName ()
{
    QGraphicsLayout *layout = 0;

    m_raitem = new RightArrowItem (MBasicListItem::IconWithTitle, 0);
    layout = m_raitem->createLayout ();

    QVERIFY (layout != 0);

    m_raitem->setTitleStyleName ("TitleStyleName");

    // XXX: test something usable here
}

QTEST_APPLESS_MAIN(Ut_DrillDownItem);
