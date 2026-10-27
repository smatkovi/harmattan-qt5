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

#include "mdesktopentry.h"

#include <QString>
#include <QStringList>
#include <MTheme>

//#define DEBUG
#define WARNING
#include "../../src/debug.h"

/******************************************************************************
 * We have a file system here! ;)
 */
static bool         fileSystemInitialized = false;
static QStringList  fileSystemFiles;


void
initFileSystem ()
{
    if (fileSystemInitialized)
        return;

    SYS_DEBUG ("Initializing file system.");
    fileSystemFiles <<
        QString(DIRNAMEDesktopFilePerfect) + "/index.theme" <<
        QString(DIRNAMEDesktopFileHidden) + "/index.theme" <<
        QString(DIRNAMEDesktopFileEmpty) + "/index.theme";

    /*
     * The third is actually a simulated fake for the current theme.
     */
    MTheme *theme = MTheme::instance();
    QString themeName = theme->currentTheme();

    fileSystemFiles << 
        QString("/usr/share/themes/") + themeName + "/index.theme";

    /*
     * Here is a perfect desktop file for the wallpaper unit tests. We simulate
     * a perfect desktop file with the same name the WallpaperBusinessLogic will
     * use to find the file. This way the WallpaperBusinessLogic can be tested
     * with a simulated content without any file  manipulation.
     */
    QString desktopfile = 
        QString(getenv("HOME")) + "/.wallpapers/wallpaper.desktop";
    fileSystemFiles << 
        desktopfile;

    /*
     * If we need to see what exactly we have in this simulated file system.
     */
    #ifdef DEBUG
    int n = 0;
    foreach (QString debugString, fileSystemFiles) {
        SYS_DEBUG ("fileSystemFiles[%d] = %s", n, SYS_STR(debugString));
        ++n;
    }
    #endif
    
    fileSystemInitialized = true;
}

MDesktopEntry::DesktopID
getDesktopID (
        const QString &filePath)
{
    MDesktopEntry::DesktopID retval;

    initFileSystem ();

    retval = (MDesktopEntry::DesktopID) fileSystemFiles.indexOf (filePath);

    #if 0
    SYS_DEBUG ("*****************************************************");
    SYS_DEBUG ("*** filePath = %s", SYS_STR(filePath));
    SYS_DEBUG ("*** retval   = %d", retval);

    switch (retval) {
        case MDesktopEntry::DesktopFileMissing:
            SYS_DEBUG ("MDesktopEntry::DesktopFileMissing");
            break;

        case MDesktopEntry::DesktopFilePerfect:
            SYS_DEBUG ("MDesktopEntry::DesktopFilePerfect");
            break;

        case MDesktopEntry::DesktopFileHidden:
            SYS_DEBUG ("MDesktopEntry::DesktopFileHidden");
            break;
        
        case MDesktopEntry::DesktopFileEmpty:
            SYS_DEBUG ("MDesktopEntry::DesktopFileEmpty");
            break;
        
        case MDesktopEntry::DesktopFileCurrent:
            SYS_DEBUG ("MDesktopEntry::DesktopFileCurrent");
            break;

        case MDesktopEntry::WallpaperdesktopFilePerfect:
            SYS_DEBUG ("MDesktopEntry::WallpaperdesktopFilePerfect");
            break;

        default:
            SYS_WARNING ("Unhandled case: %d", retval);
    }
    #endif

    return retval;
}

/******************************************************************************
 * MDesktopEntry implementation.
 */
MDesktopEntry::MDesktopEntry (
        const QString &filePath)
{
    initFileSystem ();

    SYS_DEBUG ("*** this     = %p", this);
    SYS_DEBUG ("*** filePath = %s", SYS_STR(filePath));
    m_FilePath = filePath;
}

MDesktopEntry::~MDesktopEntry ()
{
    SYS_DEBUG ("*** this     = %p", this);
}

bool
MDesktopEntry::isValid () const
{
    DesktopID id;
    bool      retval = false;

    id = getDesktopID (m_FilePath);
    SYS_DEBUG ("*** filePath = %s", SYS_STR(m_FilePath));
    SYS_DEBUG ("*** id       = %d", id);

    switch (id) {
        case DesktopFileMissing:
            break;

        case DesktopFilePerfect:
        case DesktopFileHidden:
        case DesktopFileEmpty:
        case DesktopFileCurrent:
        case WallpaperdesktopFilePerfect:
            retval = true;
            break;
    }

    SYS_DEBUG ("*** retval   = %s", SYS_BOOL(retval));
    return retval;
}

QString 
MDesktopEntry::value (
        const QString &key) const
{
    QString   value;
    DesktopID id;
    
    id = getDesktopID (m_FilePath);

    SYS_DEBUG ("*** getting %s", SYS_STR(key));
    if (id == WallpaperdesktopFilePerfect)
        value = getValueForWallp (id, key);
    else if (key == "Desktop Entry/Type")
        value = getValueForType (id); 
    else if (key == "Desktop Entry/Name")
        value = getValueForName (id); 
    else if (key == "X-MeeGoTouch-Metatheme/X-Icon")
        value = getValueForIcon (id); 
    else if (key == "X-MeeGoTouch-Metatheme/X-Visible")
        value = getValueForVisible (id); 
    else {
        SYS_WARNING ("Key not suported: %s", SYS_STR(key));
    }

    return value;
}

/******************************************************************************
 * Private functions.
 */
QString 
MDesktopEntry::getValueForType (
        DesktopID id) const
{
    QString retval;

    switch (id) {
        case DesktopFileMissing:
            // The empty string
            break;

        case DesktopFilePerfect:
        case DesktopFileHidden:
        case DesktopFileCurrent:
            retval = "X-MeeGoTouch-Metatheme";
            break;
        
        case DesktopFileEmpty:
        default:
            // All strings are empty
            break;
    }

    SYS_DEBUG ("Returning %s", SYS_STR(retval));
    return retval;
}

QString 
MDesktopEntry::getValueForName (
        DesktopID id) const
{
    QString retval;
    
    switch (id) {
        case DesktopFileMissing:
            // The empty string
            break;

        case DesktopFilePerfect:
            retval = NAMEDesktopFilePerfect;
            break;
        
        case DesktopFileHidden:
            retval = NAMEDesktopFileHidden;
            break;
        
        case DesktopFileCurrent:
            retval = NAMEDesktopFileCurrent;
            break;

        case DesktopFileEmpty:
        default:
            // All strings are empty
            break;
    }

    SYS_DEBUG ("Returning %s", SYS_STR(retval));
    return retval;
}

QString 
MDesktopEntry::getValueForIcon (
        DesktopID id) const
{
    QString retval;
    
    switch (id) {
        case MDesktopEntry::DesktopFileMissing:
            // The empty string
            break;

        case MDesktopEntry::DesktopFilePerfect:
            retval = ICONDesktopFilePerfect; 
            break;

        case MDesktopEntry::DesktopFileHidden:
            retval = ICONDesktopFileHidden;
            break;
        
        case MDesktopEntry::DesktopFileEmpty:
            // All strings are empty
            break;

        case MDesktopEntry::DesktopFileCurrent:
            retval = ICONDesktopFileCurrent;
            break;

        default:
            SYS_DEBUG ("Unhandled case");
    }

    SYS_DEBUG ("Returning %d/%s", id, SYS_STR(retval));
    return retval;
}

QString 
MDesktopEntry::getValueForVisible (
        DesktopID id) const
{
    QString retval;

    switch (id) {
        case MDesktopEntry::DesktopFileMissing:
            // The empty string
            break;

        case MDesktopEntry::DesktopFilePerfect:
            retval = "true";
            break;
        
        case MDesktopEntry::DesktopFileHidden:
            retval = "false";
            break;
        
        case MDesktopEntry::DesktopFileCurrent:
            retval = "true";
            break;

        case MDesktopEntry::DesktopFileEmpty:
        default:
            // All strings are empty
            break;
    }

    SYS_DEBUG ("Returning %s", SYS_STR(retval));
    return retval;
}

QString 
MDesktopEntry::getValueForWallp (
        DesktopID       id, 
        const QString  &key) const
{
    QString     retval;
    QStringList keysAndValues;
    int         index;

    Q_UNUSED (id);

    keysAndValues <<
        "DCP Landscape Wallpaper/EditedFile" << WALLPAPER_LANDSCAPE_EDITEDFILE <<
        "DCP Portrait Wallpaper/EditedFile"  << WALLPAPER_PORTRAIT_EDITEDFILE <<
        "DCP Landscape Wallpaper/MimeType"   << WALLPAPER_MIMETYPE <<
        "Desktop Entry/Name"                 << WALLPAPER_NAME <<
        "Desktop Entry/Version"              << WALLPAPER_VERSION <<
        "DCP Landscape Wallpaper/OriginalFile" << WALLPAPER_LANDSCAPE_ORIGINALFILE <<
        "DCP Portrait Wallpaper/OriginalFile" << WALLPAPER_PORTRAIT_ORIGINALFILE <<
        "DCP Landscape Wallpaper/HorOffset"  << WALLPAPER_LANDSCAPE_HOROFFSET <<
        "DCP Landscape Wallpaper/VertOffset" << WALLPAPER_LANDSCAPE_VERTOFFSET <<
        "DCP Portrait Wallpaper/HorOffset" << WALLPAPER_PORTRAIT_HOROFFSET <<
        "DCP Portrait Wallpaper/VertOffset" << WALLPAPER_PORTRAIT_VERTOFFSET <<
        "DCP Landscape Wallpaper/Scale" << WALLPAPER_LANDSCAPE_SCALE <<
        "DCP Portrait Wallpaper/Scale" << WALLPAPER_PORTRAIT_SCALE;


    index = keysAndValues.indexOf (key);
    if (index >= 0) {
        retval = keysAndValues[index + 1];
    } else {
        SYS_WARNING ("Key not supported: %s", SYS_STR(key));
        Q_ASSERT (!retval.isEmpty());
    }

    return retval;
}
