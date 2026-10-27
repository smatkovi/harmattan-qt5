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
#ifndef WALLPAPERCURRENTDESCRIPTOR_H
#define WALLPAPERCURRENTDESCRIPTOR_H

#include <meegocontrolexport.h>
#include "wallpaperdescriptor.h"
#include "wallpaperitrans.h"

class MDesktopEntry;

/*!
 * A class that represents the current wallpaper. Holds information about the
 * modifications the user made and about the saved files created from the
 * original image by implementing the modifications.
 *
 * This class is also a singleton; there can be only one current wallpaper.
 */
class MC_EXPORT WallpaperCurrentDescriptor : public WallpaperDescriptor {
    Q_OBJECT

public:
    ~WallpaperCurrentDescriptor ();
    
    static WallpaperCurrentDescriptor *instance ();

    virtual bool isCurrent () const;
    virtual int version () const;
    virtual QPixmap originalPixmap (M::Orientation orientation) const;
    virtual bool valid () const;

    QString editedFilename (M::Orientation orientation) const;

    bool setFromDesktopFile (
            const QString &desktopFileName,
            bool           checkFilenames = false,
            const QString &landscapeFileName = QString(""),
            const QString &portraitFileName = QString(""));

    bool setFromFilenames  (
            QString     landscapeFile,
            QString     portraitFile);

    bool setFromIDs  (
            QString     landscapeID,
            QString     portraitID);

    WallpaperITrans iTrans (M::Orientation orientation) const;

protected:
    WallpaperCurrentDescriptor ();

private:
    bool getValue (
            const QString   &group,
            const QString   &key, 
            QString         &value);

    bool getValue (
            const QString   &group,
            WallpaperITrans &value);

    bool getValue (
            const QString   &group,
            const QString   &key, 
            qreal           *value);
    
private:
    static WallpaperCurrentDescriptor *sm_Instance;

    bool                   m_Valid;
    MDesktopEntry         *m_DesktopEntry;
    WallpaperITrans        m_LandscapeTrans;
    WallpaperITrans        m_PortraitTrans;
    int                    m_Version;
    #ifdef UNIT_TEST
    friend class Ut_WallpaperCurrentDescriptor;
    #endif
};

#endif
