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
#include "ut_displaywidget.h"
#include "displaywidget.h"

#include "qmdisplaystate.h"
#ifndef QMDISPLAYSTATE_STUB_H
#  error "QmDisplayState should be stubbed, can nopt continue."
#endif

#include "displaybusinesslogic.h"

#include <MApplication>
#include <MGConfItem>
#include <MComboBox>
#include <MDialog>
#include <MButton>
#include <MLabel>
#include <QHash>

#include <QList>
#include <QVariant>

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Stubbing the MGConfItem.
 */
static bool dimTimeoutsExist = true;

QVariant
MGConfItem::value () const
{
    SYS_DEBUG ("*** key() = %s", SYS_STR(key()));

    if (key() == "/meegotouch/debug/show_async_requests")
        return QVariant (false);

    if (key() == "/meegotouch/target/name")
        return QVariant ("Default");

    if (key() != "/system/osso/dsm/display/possible_display_dim_timeouts") {
        SYS_DEBUG ("%s ???", SYS_STR(key()));
        goto return_invalid;
    }

    if (dimTimeoutsExist) {
        QList<QVariant> list;

        list << 
            QVariant (15) << 
            QVariant (30) << 
            QVariant (60) << 
            QVariant (120) <<
            QVariant (180);

        return QVariant (list);
    }

return_invalid:
    return QVariant ();
}

/******************************************************************************
 * Ut_DisplayWidget implementation. 
 */
void 
Ut_DisplayWidget::init()
{
}

void 
Ut_DisplayWidget::cleanup()
{
}


int argc = 1;
char* app_name[] = { (char*) "./ut_displayapplet", 0 };

void 
Ut_DisplayWidget::initTestCase()
{
    m_App = new MApplication (argc, app_name);
}

void 
Ut_DisplayWidget::cleanupTestCase()
{
    delete m_App;
}

/*!
 * This test will check that when the screen timeout widget is clicked a dialog
 * will be shown with the possible values and when a value is selected the
 * screen light value has been set for the businesslogic.
 */
void 
Ut_DisplayWidget::testScreenTimeout ()
{
    DisplayWidget *widget;
    int            screenLightsValue;
    int            SelectIndex;

    widget = new DisplayWidget;
    QVERIFY (widget->m_logic);
    QVERIFY (widget->m_screenTimeout);
   
    /*
     * Go through the values by accepting every one of them.
     */
    for (SelectIndex = 0; SelectIndex <= 4; ++SelectIndex) {
        widget->m_screenTimeout->click();

        // simulate click on selected value...
        widget->m_screenTimeout->setProperty ("currentIndex", SelectIndex);
        
        screenLightsValue = widget->m_logic->selectedScreenLightsValue ();
        SYS_DEBUG ("*** screenLightsValue = %d", screenLightsValue);
        QCOMPARE (screenLightsValue, SelectIndex);
    }

    /*
     * Then the same with rejecting the dialog.
     */
    for (SelectIndex = 0; SelectIndex <= 4; ++SelectIndex) {
        widget->m_screenTimeout->click();

        screenLightsValue = widget->m_logic->selectedScreenLightsValue ();
        SYS_DEBUG ("*** screenLightsValue = %d", screenLightsValue);
        QCOMPARE (screenLightsValue, 4);
    }

    delete widget;
}

void 
Ut_DisplayWidget::testTranslation ()
{
    DisplayWidget *widget;

    widget = new DisplayWidget;
    widget->retranslateUi ();

    if (widget->m_TitleLabel)
        QCOMPARE (widget->m_TitleLabel->text(), qtTrId ("qtn_disp_display"));

    if (widget->m_SubTitleLabel)
        QCOMPARE (widget->m_SubTitleLabel->text(), qtTrId ("qtn_disp_bright"));

    if (widget->m_screenTimeout)
        QCOMPARE (widget->m_screenTimeout->title(), qtTrId ("qtn_disp_screenoff"));
}

QTEST_APPLESS_MAIN(Ut_DisplayWidget)
