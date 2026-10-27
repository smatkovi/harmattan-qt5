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

#include "ut_themelistmodel.h"
#include "mdesktopentry.h"
#include "themebusinesslogic.h"
#include "themelistmodel.h"
#include "themedescriptor.h"

#include <MApplication>
#include <MGConfItem>
#include <MTheme>

#ifndef MDESKTOPENTRY_STUB_H
# error "The MDesktopEntry class is not stubbed, can't continue."
#endif

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Fake MGconfItem implementation. 
 */
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
 * Ut_ThemeListModelPrivate implementation. 
 */
UtThemeListModelPrivate::UtThemeListModelPrivate ()
{
    reset ();
}


void
UtThemeListModelPrivate::reset ()
{
    m_FirstChanged = QModelIndex ();
    m_LastChanged = QModelIndex ();
}

void
UtThemeListModelPrivate::dataChanged (
        const QModelIndex &first, 
        const QModelIndex &last)
{
    SYS_DEBUG ("");
    m_FirstChanged = first;
    m_LastChanged = last;
}

/******************************************************************************
 * Ut_ThemeListModel implementation. 
 */
void 
Ut_ThemeListModel::init()
{
}

void 
Ut_ThemeListModel::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_ThemeListModel";

void 
Ut_ThemeListModel::initTestCase()
{
    bool connectSuccess;

    m_Priv = new UtThemeListModelPrivate;
    m_App = new MApplication (argc, &app_name);
    m_Logic = ThemeBusinessLogic::instance();
    m_Api = new ThemeListModel (m_Logic);

    connectSuccess = connect (
            m_Api, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            m_Priv, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
    QVERIFY (connectSuccess);
}

void 
Ut_ThemeListModel::cleanupTestCase()
{
    delete m_App;
    delete m_Api;
    delete m_Logic;
    delete m_Priv;
}

/*!
 * This test will go around and check the data it finds in the model. The data
 * that is actually there created by our stub implementations...
 */
void
Ut_ThemeListModel::testAvailableThemes ()
{
    QModelIndex  index;
    QString      dataDisplayRole, dataSearchRole, dataCodeNameRole;
    QString      dataNameRole, dataIconNameRole, dataChangingNameRole;
    int          rows = m_Api->rowCount ();
    QVariant     data;

    SYS_DEBUG ("*** rows    = %d", rows);

    // This much theme is stored in the stub we use.
    QVERIFY (rows == 2);

    index = m_Api->index (0, 0);
    dataDisplayRole = m_Api->data (index, Qt::DisplayRole).toString();
    dataSearchRole = m_Api->data (index, ThemeListModel::SearchRole).toString();
    dataCodeNameRole = m_Api->data (index, ThemeListModel::CodeNameRole).toString();
    dataNameRole = m_Api->data (index, ThemeListModel::NameRole).toString();
    dataIconNameRole = m_Api->data (index, ThemeListModel::IconNameRole).toString();
    dataChangingNameRole = m_Api->data (index, ThemeListModel::ChangingNameRole).toString();

    SYS_DEBUG ("*** dataDisplayRole   = %s", SYS_STR(dataDisplayRole));
    SYS_DEBUG ("*** dataSearchRole    = %s", SYS_STR(dataSearchRole));
    SYS_DEBUG ("*** dataCodeNameRole  = %s", SYS_STR(dataCodeNameRole));
    SYS_DEBUG ("*** dataNameRole      = %s", SYS_STR(dataNameRole));
    SYS_DEBUG ("*** dataIconNameRole  = %s", SYS_STR(dataIconNameRole));
    SYS_DEBUG ("*** dataChangingNameRole = %s", SYS_STR(dataChangingNameRole));

    QVERIFY (dataDisplayRole == NAMEDesktopFilePerfect);
    QVERIFY (dataSearchRole == NAMEDesktopFilePerfect);
    QVERIFY (dataCodeNameRole == CODENAMEDesktopFilePerfect);
    QVERIFY (dataNameRole == NAMEDesktopFilePerfect);
    QVERIFY (dataChangingNameRole.isEmpty());

    /*
     * Invalid role.
     */
    data = m_Api->data (index, ThemeListModel::ChangingNameRole + 12);
    QVERIFY (!data.isValid());

    /*
     * Invalid index.
     */
    index = m_Api->index (100, 100);
    data = m_Api->data (index, Qt::DisplayRole);
    QVERIFY (!data.isValid());

    /*
     * Finding an index by the theme code name.
     */
    index = m_Api->indexOfCodeName (CODENAMEDesktopFilePerfect);
    QVERIFY (index.isValid());
    QVERIFY (index.row() == 0);

    dataCodeNameRole = m_Api->data (
            index, ThemeListModel::CodeNameRole).toString();
    QVERIFY (dataCodeNameRole == CODENAMEDesktopFilePerfect);
}

/*!
 * This test will check the refresh() public method of the model. The refresh()
 * should always send a signal to refresh all the items in the model.
 */
void
Ut_ThemeListModel::testRefresh ()
{
    m_Priv->reset ();
    m_Api->refresh ();
    
    QVERIFY (m_Priv->m_FirstChanged.isValid());
    QVERIFY (m_Priv->m_FirstChanged.row() == 0);

    // We stubbed 2 items...
    QVERIFY (m_Priv->m_LastChanged.isValid());
    QVERIFY (m_Priv->m_LastChanged.row() == 1);
}

QTEST_APPLESS_MAIN(Ut_ThemeListModel)


