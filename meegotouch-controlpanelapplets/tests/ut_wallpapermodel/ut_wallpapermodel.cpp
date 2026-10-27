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

#include "ut_wallpapermodel.h"
#include "wallpaperutils_stub.h"

#include "wallpapermodel.h"
#include "wallpaperdescriptor.h"
#include "wallpaperbusinesslogic.h"
#include "wallpaperconfiguration.h"
#include "wallpaperutils.h"

#include <QSet>
#include <MGConfItem>
#include <MApplication>
#include <QFileInfo>

#define DEBUG
#include "../../src/debug.h"

static const QString PortraitKey =
    "/desktop/meego/background/portrait/picture_filename";
static const QString LandscapeKey =
    "/desktop/meego/background/landscape/picture_filename";

static const QString testFile1 ("/testFile1.png");
static const QString testFile2 ("/testFile2.png");
static const QString testFile3 ("/testFile3.png");

/******************************************************************************
 * Stub for QFileInfo.size () needed as empty files are skipped 
 */
qint64
QFileInfo::size () const
{
    return 5120;
}

/******************************************************************************
 * WallpaperModelSignals implementation. 
 */
void
WallpaperModelSignals::currentChanged (
        const QModelIndex &current)
{
    Q_UNUSED (current);
    SYS_DEBUG ("");
}

/******************************************************************************
 * Ut_WallpaperModel implementation. 
 */
void 
Ut_WallpaperModel::init()
{
    MGConfItem gconf_portrait (PortraitKey);
    MGConfItem gconf_landscape (LandscapeKey);

    /* Reset the wallpaper to the default */
    gconf_portrait.unset ();
    gconf_landscape.unset ();

    /* SBOX maybe does not have some default value */
    if (gconf_portrait.value ().toString ().isEmpty ())
        gconf_portrait.value ().setValue (testFile1);
}

void 
Ut_WallpaperModel::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./ut_wallpapermodel";

void 
Ut_WallpaperModel::initTestCase()
{
    bool signalSuccess;

    stubAddFileToFilesystem (testFile1, 5120);
    stubAddFileToFilesystem (testFile2, 5120);

    m_App = new MApplication (argc, &app_name);
    m_BusinessLogic = new WallpaperBusinessLogic;
    m_Model = new WallpaperModel (m_BusinessLogic);

    signalSuccess = connect (
            m_Model, SIGNAL(currentChanged(const QModelIndex &)),
            &m_Signals, SLOT(currentChanged(const QModelIndex &)));
    QVERIFY (signalSuccess);

    QVERIFY (m_Model->m_ImagesDir == 
            Wallpaper::constructPath (Wallpaper::ImagesDir));
    QVERIFY (m_Model->m_SysImagesDir == 
            Wallpaper::constructPath (Wallpaper::SysImagesDir));

    #if 0
    SYS_DEBUG ("*** rowCount() = %d", m_Model->rowCount());
    for (int n = 0; n < m_Model->m_FilePathList.size(); ++n) {
        SYS_DEBUG ("[%03d] %s", n, SYS_STR(m_Model->m_FilePathList[n]));
    }
    #endif
    
    /*
     * Re-calling the loadFromDirectory() is delayed, but we force calling it
     * now.
     */
    m_Model->loadFromDirectory ();

    SYS_DEBUG ("*** rowCount() = %d", m_Model->rowCount());
    for (int n = 0; n < m_Model->m_FilePathList.size(); ++n) {
        SYS_DEBUG ("[%03d] %s", n, SYS_STR(m_Model->m_FilePathList[n]));
    }

    /*
     * We must have the two simulated files, and maybe the current image
     * (it is varying based on the environment (sbox, strange test images...)
     */
    QVERIFY(m_Model->rowCount() >= 2);
}

void 
Ut_WallpaperModel::cleanupTestCase()
{
    delete m_BusinessLogic;
    delete m_Model;
    m_App->deleteLater ();
}

void 
Ut_WallpaperModel::testData ()
{
    QSet<int> theSet;

    for (int n = 0; n < m_Model->rowCount(); ++n) {
        QModelIndex index = m_Model->index (n, 0);
        QVariant    data = index.data(WallpaperModel::WallpaperDescriptorRole);
        WallpaperDescriptor desc = data.value<WallpaperDescriptor>();

        SYS_DEBUG ("[%03d] desc = %s", n, SYS_STR(desc.filePath()));
        SYS_DEBUG ("[%03d] intr = %s", n, SYS_STR(m_Model->m_FilePathList[n]));
        QVERIFY (desc.filePath() == m_Model->m_FilePathList[n]);
    }

    for (int n = 0; n < m_Model->rowCount(); ++n) {
        QModelIndex index = m_Model->index (n, 0);
        QVariant    data;
        data = index.data(Qt::DisplayRole);
        int         iData= data.toInt ();

        SYS_DEBUG ("[%03d] order= %d", n, iData);
        SYS_DEBUG ("[%03d] intr = %s", n, SYS_STR(m_Model->m_FilePathList[n]));
        QVERIFY (!theSet.contains(iData));
        theSet.insert (iData);
    }
}

void
Ut_WallpaperModel::testSelectByFilepath ()
{
    m_Model->selectByFilepath (testFile1);
    QVERIFY(m_Model->selectedPath() == testFile1);

    /*
     * We "create" the file and then we re-load the directory. 
     * FIXME: What happens when the selectByFilepath() is called before the
     * model has the chance to read the directory twice?
     */
    stubAddFileToFilesystem (testFile3, 1000);
    m_Model->loadFromDirectory ();
    m_Model->loadFromDirectory ();

    /*
     * Now we select the file and check if it is selected.
     */
    m_Model->selectByFilepath (testFile3);
    QVERIFY(m_Model->selectedPath() == testFile3);
}

QTEST_APPLESS_MAIN(Ut_WallpaperModel)


