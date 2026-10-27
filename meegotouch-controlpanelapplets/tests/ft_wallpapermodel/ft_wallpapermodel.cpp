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

#include "ft_wallpapermodel.h"

#include "wallpaperbusinesslogic.h"
#include "wallpapermodel.h"
#include "wallpapercurrentdescriptor.h"
#include "wallpaperitrans.h"

#include <QPixmap>

#include <MApplication>
#include <MGConfItem>

#define DEBUG
#include "../../src/debug.h"


/******************************************************************************
 * SignalSink implementation.
 */
SignalSink::SignalSink ()
{
    reset ();
}

void 
SignalSink::reset ()
{
    m_WallpaperChangedCame = false;
}

void 
SignalSink::wallpaperChanged ()
{
    SYS_DEBUG ("");
    m_WallpaperChangedCame = true;
}

/******************************************************************************
 * Ft_WallpaperModel implementation. 
 */
void 
Ft_WallpaperModel::init()
{
}

void 
Ft_WallpaperModel::cleanup()
{
}


static int argc = 1;
static char* app_name = (char*) "./Ft_WallpaperModel";

void 
Ft_WallpaperModel::initTestCase()
{
    bool connectSuccess;

    m_App = new MApplication (argc, &app_name);
    m_BusinessLogic = new WallpaperBusinessLogic;
    m_Model = new WallpaperModel (m_BusinessLogic);

    connectSuccess = connect (
            m_BusinessLogic, SIGNAL (wallpaperChanged()),
            &m_SignalSink, SLOT (wallpaperChanged()));
    QVERIFY (connectSuccess);
}

void 
Ft_WallpaperModel::cleanupTestCase()
{
    delete m_Model;
    delete m_BusinessLogic;

    m_App->deleteLater ();
}

void 
Ft_WallpaperModel::testRowsColumns ()
{
    QModelIndex    index;
    int            columns = m_Model->columnCount (index);
    int            rows = m_Model->rowCount (index);

    SYS_DEBUG ("*** columns = %d", columns);
    SYS_DEBUG ("*** rows    = %d", rows);

    QVERIFY (columns == 1);
    QVERIFY (rows >= 1);
}

void 
Ft_WallpaperModel::testData ()
{
    QModelIndex          index;
    QVariant             data;
    WallpaperDescriptor *desc;
    int            rows = m_Model->rowCount (index);

    for (int n = 0; n < rows; ++n) {
        index = m_Model->index (n, 0);
        data = index.data(WallpaperModel::WallpaperDescriptorRole);
        desc = data.value<WallpaperDescriptor*>();

        #if 0
        SYS_DEBUG ("--- index at %d -----------------", n);
        SYS_DEBUG ("*** position = %d, %d", index.column(), index.row());
        SYS_DEBUG ("*** file     = %s", SYS_STR(desc->filename()));
        #endif
        
        QVERIFY (index.isValid ());
        QVERIFY (index.column() == 0);
        QVERIFY (index.row() == n);
        QVERIFY (index.model() == m_Model);
        QVERIFY (isWallpaperDescriptorValid(desc));
    }
}

bool
Ft_WallpaperModel::isWallpaperDescriptorValid (
        WallpaperDescriptor *desc)
{
    QString filename, basename, mimeType;
    bool valid = true;

    if (desc == 0) {
        SYS_WARNING ("The pointer should not be NULL.");
        return false;
    }
   
    /*
     * If this is a theme based wallpaper the image informations might be empty.
     */
    if (!desc->imageID().isEmpty())
        goto no_file_check_necessary;

    /*
     * Checking the filename
     */
    filename = desc->filename();
    if (filename.isEmpty())
        valid = false;
    else if (!filename.startsWith("/"))
        valid = false;
    else if (filename.endsWith("/"))
        valid = false;
    else if (filename.contains("*") || filename.contains("?"))
        valid = false;

    if (!valid) {
        SYS_WARNING ("Invalid filename: %s", SYS_STR(filename));
        return false;
    }

    /*
     * Checking basename
     */
    basename = desc->basename();
    if (basename.isEmpty())
        valid = false;
    else if (basename.contains("/"))
        valid = false;
    else if (basename.contains("*") || filename.contains("?"))
        valid = false;

    if (!valid) {
        SYS_WARNING ("Invalid basename: %s", SYS_STR(basename));
        return false;
    }

    /*
     * Checking the MimeType
     */
    mimeType = desc->mimeType ();
    if (mimeType.isEmpty())
        valid = false;
    else if (!mimeType.startsWith("image/"))
        valid = false;

    if (!valid) {
        SYS_WARNING ("Invalid mimeType = %s", SYS_STR(mimeType));
        return false;
    }

no_file_check_necessary:

    return true;
}

QTEST_APPLESS_MAIN(Ft_WallpaperModel)



