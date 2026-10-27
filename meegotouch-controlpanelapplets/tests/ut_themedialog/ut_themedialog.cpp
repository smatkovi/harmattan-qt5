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

#include "ut_themedialog.h"

#include "themedialog.h"
#include "themedescriptor.h"

#include <MApplication>
#include <MDialog>
#include <MTheme>
#include <MGConfItem>
#include <MButton>

#include <QModelIndex>
#include <QVariant>
#include <QPointer>

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
 * Fake MGconfItem implementation. 
 */
static const QString themeKey = "/meegotouch/theme/name";
QString  GConfLastKey;
QString  GConfLastValue;

void
MGConfItem::set (
        const QVariant &val)
{
    GConfLastKey   = key();
    GConfLastValue = val.toString();
    SYS_DEBUG ("*** value = %s", SYS_STR(val.toString()));
    SYS_DEBUG ("*** key   = %s", SYS_STR(key()));
}


/******************************************************************************
 * Ut_ThemeDialog implementation. 
 */
void 
Ut_ThemeDialog::init()
{
}

void 
Ut_ThemeDialog::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_ThemeDialog";

void 
Ut_ThemeDialog::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_ThemeBusinessLogic = ThemeBusinessLogic::instance();
}

void 
Ut_ThemeDialog::cleanupTestCase()
{
    delete m_ThemeBusinessLogic;

    m_App->deleteLater ();
}

void 
Ut_ThemeDialog::testAccept ()
{
    QList<ThemeDescriptor *>   themes;
    QPointer<ThemeDialog>      dialog;
    
    /*
     * Getting the themes so we have at least one theme descriptor. The data
     * source is stubbed, no file manpulation here.
     */
    themes = m_ThemeBusinessLogic->availableThemes ();
    QVERIFY (themes.size() > 0);

    /*
     * Creating and showing the dialog. The MDialog is stubbed, no window will
     * appear here.
     */
    dialog = new ThemeDialog (m_ThemeBusinessLogic, themes[0]);
    dialog->showDialog ();
    QVERIFY (dialog->m_SelectButton);
    QVERIFY (dialog->m_CancelButton);
    QCOMPARE (lastDialogTitle, themes[0]->name());

    /*
     * Clicking the button and checking if everything is all right. The
     * MGConfitem is stubbed, no file manipulation here.
     */
    GConfLastKey = "";
    GConfLastValue = "";
    //dialog->m_SelectButton->click();

    dialog->accept ();
    dialog->dialogDisappeared ();

    QCOMPARE (GConfLastKey, themeKey);
    QCOMPARE (GConfLastValue, themes[0]->codeName());
}

void 
Ut_ThemeDialog::testCancel ()
{
    QList<ThemeDescriptor *>   themes;
    QPointer<ThemeDialog>      dialog;

    /*
     * Getting the themes so we have at least one theme descriptor. The data
     * source is stubbed, no file manpulation here.
     */
    themes = m_ThemeBusinessLogic->availableThemes ();
    QVERIFY (themes.size() > 0);

    /*
     * Creating and showing the dialog. The MDialog is stubbed, no window will
     * appear here.
     */
    dialog = new ThemeDialog (m_ThemeBusinessLogic, themes[0]);
    dialog->showDialog ();
    QVERIFY (dialog->m_SelectButton);
    QVERIFY (dialog->m_CancelButton);
    QCOMPARE (lastDialogTitle, themes[0]->name());

    /*
     * Clicking the button and checking if everything is all right. The
     * MGConfitem is stubbed, no file manipulation here.
     */
    GConfLastKey = "";
    GConfLastValue = "";
    dialog->m_CancelButton->click();
    QCOMPARE (GConfLastKey, QString (""));
    QCOMPARE (GConfLastValue, QString (""));
}

QTEST_APPLESS_MAIN(Ut_ThemeDialog)


