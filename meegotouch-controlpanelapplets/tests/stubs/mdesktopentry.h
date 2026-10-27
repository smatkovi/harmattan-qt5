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

#ifndef MDESKTOPENTRY_H_
#define MDESKTOPENTRY_H_
#define MDESKTOPENTRY_STUB_H

#include <QObject>
#include <QFlags>

#ifdef FUNCTIONAL_TEST
#  error "This header should not be used in functional tests."
#endif
#ifndef UNIT_TEST
#  error "This header should be used only in unit tests."
#endif

/*
 * We return an invalid MDesktopEntry for this. Actually any values that are not
 * enlisted below would suffice.
 */
#define DIRNAMEDesktopFileMissing  "/nosuchfile"
#define CODENAMEDesktopFileMissing "nosuchentry"

/*
 * With this we return strings for every keys.
 */
#define DIRNAMEDesktopFilePerfect  "/usr/share/themes/planktonfake"
#define CODENAMEDesktopFilePerfect "planktonfake"
#define NAMEDesktopFilePerfect "DesktopFilePerfectName"
#define ICONDesktopFilePerfect "DesktopFilePerfectIcon"

/*
 * For these we return strings that represent a hidden theme; all strings are
 * valid, but the theme is hidden.
 */
#define DIRNAMEDesktopFileHidden  "/usr/share/themes/basefake"
#define CODENAMEDesktopFileHidden "basefake"
#define NAMEDesktopFileHidden "DesktopFileHiddenName"
#define ICONDesktopFileHidden "DesktopFileHiddenIcon"

/*
 * For these all the keys will hold no value, we return the empty string.
 */
#define DIRNAMEDesktopFileEmpty  "/usr/share/themes/duifake"
#define CODENAMEDesktopFileEmpty "duifake"

/*
 * These will be used to emulate the current theme whatever is its name.
 */
#define NAMEDesktopFileCurrent "DesktopFileCurrentName"
#define ICONDesktopFileCurrent "DesktopFileCurrentIcon"

/*
 * 
 */
#define WALLPAPER_DESKTOPFILE_MISSING "/no_such_a_dir/nor_the_file.desktop"

/*
 * The content of a perfectly shaped desktop file containing information about
 * the saved wallpaper. the filename is calculated from the users home directory
 * name in the initFileSystem() function.
 */
#define WALLPAPER_LANDSCAPE_EDITEDFILE "/somewhere/WallpaperLandscapeEditedFile.jpeg"
#define WALLPAPER_PORTRAIT_EDITEDFILE "/somewhere/WallpaperLandscapeEditedFile.jpeg"
#define WALLPAPER_MIMETYPE "image/jpeg"
#define WALLPAPER_NAME "Wallpaper name/title"
#define WALLPAPER_VERSION "27"
#define WALLPAPER_LANDSCAPE_ORIGINALFILE "/usr/share/LandscapeOriginalFile.jpeg"
#define WALLPAPER_PORTRAIT_ORIGINALFILE "/usr/share/PortraitOriginalFile.jpeg"

#define WALLPAPER_LANDSCAPE_HOROFFSET_NUM 33
#define WALLPAPER_LANDSCAPE_HOROFFSET "33"

#define WALLPAPER_LANDSCAPE_VERTOFFSET_NUM 100
#define WALLPAPER_LANDSCAPE_VERTOFFSET "100"

#define WALLPAPER_PORTRAIT_HOROFFSET_NUM 18
#define WALLPAPER_PORTRAIT_HOROFFSET "18"
#define WALLPAPER_PORTRAIT_VERTOFFSET_NUM 17
#define WALLPAPER_PORTRAIT_VERTOFFSET "17"

#define WALLPAPER_LANDSCAPE_SCALE "0.9"
#define WALLPAPER_PORTRAIT_SCALE "1.1"

class QString;

/*!
 * This file contains the definition for a simulated MDesktopentry database that
 * holds simulated values for some desktop files.
 */
class MDesktopEntry: public QObject 
{
Q_OBJECT
public:
    MDesktopEntry (const QString &filePath);
    ~MDesktopEntry ();

    bool isValid () const;

    typedef enum {
        DesktopFileMissing = -1,
        DesktopFilePerfect = 0,
        DesktopFileHidden  = 1,
        DesktopFileEmpty   = 2,
        DesktopFileCurrent = 3,
        WallpaperdesktopFilePerfect = 4,
    } DesktopID;

    QString value (const QString &key) const;

private:
    QString getValueForType    (DesktopID id) const;
    QString getValueForName    (DesktopID id) const;
    QString getValueForIcon    (DesktopID id) const;
    QString getValueForVisible (DesktopID id) const;
    QString getValueForWallp   (DesktopID id, const QString &key) const;
    QString   m_FilePath;
};
#else
#  ifndef MDESKTOPENTRY_STUB_H
#  error "The MDESKTOPENTRY_H_ is defined but the MDESKTOPENTRY_STUB_H is not. This means a foreign mdesktopentry.h was already loaded!"
#  endif
#endif

