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

#include "ut_themewidget.h"

#include "themeapplet.h"
#include "themewidget.h"
#include "themedescriptor.h"

#include <MApplication>
#include <MDialog>
#include <MTheme>
#include <MSortFilterProxyModel>
#include <QModelIndex>
#include <QVariant>

//#define LOTDEBUG
#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Stubbing the MDialog, so the UI will not be really shown.
 */
QString lastDialogTitle;

void
MDialog::appear (
        MSceneWindow::DeletionPolicy policy)
{
    SYS_DEBUG ("----------------------------------------------------------");
    SYS_DEBUG ("*** this    = %p", this);
    SYS_DEBUG ("*** title   = %s", SYS_STR(title()));

    Q_UNUSED (policy);

    lastDialogTitle = title();
}
/******************************************************************************
 * Ut_ThemeWidget implementation. 
 */
void 
Ut_ThemeWidget::init()
{
}

void 
Ut_ThemeWidget::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_ThemeWidget";

void 
Ut_ThemeWidget::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_ThemeBusinessLogic = ThemeBusinessLogic::instance();
    m_ThemeWidget = new ThemeWidget (m_ThemeBusinessLogic);
}

void 
Ut_ThemeWidget::cleanupTestCase()
{
    delete m_ThemeWidget;
    delete m_ThemeBusinessLogic;

    m_App->deleteLater ();
}

/*!
 * This test will call the themeActivated() method of the widget with each and
 * every one of the rows found in the model. Then the method will check that the
 * widget shown a dialog. This test is using a simple MDialog stub found in this
 * source file.
 */
void 
Ut_ThemeWidget::testThemeActivated ()
{
    QModelIndex      index;
    QVariant         data;
    QString          name;
    int              rows;

    rows = m_ThemeWidget->m_Proxy->rowCount (index);

    SYS_DEBUG ("*** rows    = %d", rows);
    for (int n = 0; n < rows; ++n) {
        index = m_ThemeWidget->m_Proxy->index (n, 0);

        name = index.data().toString();
        SYS_DEBUG ("*** name            = %s", SYS_STR(name));

        QVERIFY (index.isValid ());
        QVERIFY (index.column() == 0);
        QVERIFY (index.row() == n);

        lastDialogTitle = "";
        m_ThemeWidget->themeActivated (index);
        /*
         * If this is the current theme there should be no dialog shown, if not,
         * a dialog should be shown with the theme name set as title.
         */
        SYS_DEBUG ("*** lastDialogTitle = %s", SYS_STR(lastDialogTitle));
        if (m_ThemeBusinessLogic->currentThemeName() != name)
            QVERIFY (lastDialogTitle == name);
        else
            QVERIFY (lastDialogTitle == "");
    }
}

QTEST_APPLESS_MAIN(Ut_ThemeWidget)


