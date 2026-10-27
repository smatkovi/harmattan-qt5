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

#include "wallpapercurrentdescriptor.h"

#include <QPointF>
#include <QFileInfo>
#include <MTheme>

#include <mdesktopentry.h>

//#define DEBUG
//#define WARNING
//#define LOTDEBUG
#include "../debug.h"

WallpaperCurrentDescriptor *WallpaperCurrentDescriptor::sm_Instance = 0;

static const QString mainGroupKey = "Desktop Entry";
static const QString landscapeGroupKey = "DCP Landscape Wallpaper";
static const QString portraitGroupKey = "DCP Portrait Wallpaper";

static const QString versionKey = "Version";
static const QString originalFilenameKey = "OriginalFile";
static const QString editedFilenameKey = "EditedFile";
static const QString mimeTypeKey = "MimeType";
static const QString horOffsetKey = "HorOffset";
static const QString vertOffsetKey = "VertOffset";
static const QString scaleKey = "Scale";

/*!
 * \returns the current wallpaper instance (as only one can be the current)
 */
WallpaperCurrentDescriptor *
WallpaperCurrentDescriptor::instance ()
{
    if (!WallpaperCurrentDescriptor::sm_Instance)
        WallpaperCurrentDescriptor::sm_Instance = 
            new WallpaperCurrentDescriptor;

    return WallpaperCurrentDescriptor::sm_Instance;
}

/*!
 * The default constuctor implementation for the WallpaperCurrentDescriptor
 */
WallpaperCurrentDescriptor::WallpaperCurrentDescriptor () :
    m_Valid (false),
    m_DesktopEntry (0),
    m_Version (0)
{
    m_LandscapeTrans.setOrientation (M::Landscape);
    m_PortraitTrans.setOrientation (M::Portrait);
}

/*!
 * The destructor of the WallpaperCurrentDescriptor
 */
WallpaperCurrentDescriptor::~WallpaperCurrentDescriptor ()
{
    WallpaperCurrentDescriptor::sm_Instance = 0;
}

/*!
 * For this class it is always the current wallpaper.
 */
bool
WallpaperCurrentDescriptor::isCurrent () const
{
    return true;
}

/*!
 * \returns The version number of the wallpaper.
 *
 * In order to guaranty that the image file is always created by a new name (so
 * that the gconf entry change can be recognized) the current wallpaper always
 * have a version number stored in the desktop file.
 */
int
WallpaperCurrentDescriptor::version () const 
{
    return m_Version;
}

/*!
 * \param desktopFileName The full path of the desktop file to read
 * \param landscapeFileName If provided the method will report failure when the
 *   desktop file contains a different file name.
 * \param portraitFileName If provided the method will report failure when the
 *   desktop file contains a different file name.
 * 
 * Reads the data stored in a desktop wile and initializes the object using the
 * data found there.
 */
bool 
WallpaperCurrentDescriptor::setFromDesktopFile (
        const QString &desktopFileName,
        bool           checkFilenames,
        const QString &landscapeFileName,
        const QString &portraitFileName)
{
    bool           success1, success2;
    QString        value1("");
    QString        value2("");
    qreal          rval;
    bool           retval = false;

    SYS_DEBUG ("*** desktopFileName   = %s", SYS_STR(desktopFileName));
    SYS_DEBUG ("*** landscapeFileName = %s", SYS_STR(landscapeFileName));
    SYS_DEBUG ("*** portraitFileName  = %s", SYS_STR(portraitFileName));
    SYS_DEBUG ("*** checkFilenames    = %s", SYS_BOOL(checkFilenames));
    m_DesktopEntry = new MDesktopEntry (desktopFileName);

    if (!m_DesktopEntry->isValid()) {
        goto finalize;
    }

    /*
     * The edited image file name... FIXME: This is actually depends on the
     * orientation.
     * We should compare these with the values stored in the gconf database.
     */
    success1 = getValue(portraitGroupKey, editedFilenameKey, value1);
    success2 = getValue(landscapeGroupKey, editedFilenameKey, value2);
    if (!success1 && !success2) {
        SYS_DEBUG ("Non of the variants are accessed, giving up.");
        goto finalize;
    }

    /*
     * If the filenames are provided we check the desktop file data against
     * them. Even if we support only one orientation one of the filenames must
     * match.
     */
    if (checkFilenames) {
        if ((landscapeFileName.isEmpty() && portraitFileName != value1) ||
            (portraitFileName.isEmpty() && landscapeFileName != value2) ||
            (portraitFileName != value1 && landscapeFileName != value2)) {
            SYS_DEBUG ("Non of the variants are set, giving up.");
            goto finalize;
        }
    }
    
    SYS_DEBUG ("Accepting value1 = %s", SYS_STR(value1));
    SYS_DEBUG ("Accepting value2 = %s", SYS_STR(value2));
    SYS_DEBUG ("*** portraitFileName  = %s", SYS_STR(portraitFileName));
    SYS_DEBUG ("*** landscapeFileName = %s", SYS_STR(landscapeFileName));
    setFilename (value1, WallpaperDescriptor::Portrait);
    setFilename (value2, WallpaperDescriptor::Landscape);

    /*
     * FIXME: Still no support for separate files in the parent class...
     */
    setFilename (value1.isEmpty() ? value2 : value1);
    setUrl ("file://" + (value1.isEmpty() ? value2 : value1));

    /*
     * MimeType. FIXME: This should not depend on the orientation?
     */
    if (getValue(landscapeGroupKey, mimeTypeKey, value1)) {
        setMimeType (value1);
    }

    /*
     * The version number.
     */
    if (getValue(mainGroupKey, versionKey, &rval))
        m_Version = (int) rval;

    getValue (landscapeGroupKey, originalFilenameKey, value1);
    setFilename (value1, WallpaperDescriptor::OriginalLandscape);

    getValue (portraitGroupKey, originalFilenameKey, value1);
    setFilename (value1, WallpaperDescriptor::OriginalPortrait);

    getValue (landscapeGroupKey, m_LandscapeTrans);
    getValue (portraitGroupKey, m_PortraitTrans);

    retval = true;
    m_Valid = true;

finalize:
    delete m_DesktopEntry;
    m_DesktopEntry = 0;

    return retval;
}

/*!
 * Method for initialize the current-wallpaper-descriptor from filenames
 *
 * \param landscapeFile The landscape wallpaper file-name
 * \param portraitFile The portrait wallpaper file-name
 */
bool 
WallpaperCurrentDescriptor::setFromFilenames  (
            QString     landscapeFile,
            QString     portraitFile)
{
    bool retval = false;

    SYS_DEBUG ("*** landscapeFile = %s", SYS_STR(landscapeFile));
    SYS_DEBUG ("*** portraitFile  = %s", SYS_STR(portraitFile));
   
    if (!landscapeFile.isEmpty() && !landscapeFile.startsWith("/"))
        goto finalize;

    if (!portraitFile.isEmpty() && !portraitFile.startsWith("/"))
        goto finalize;


    m_LandscapeTrans = WallpaperITrans();
    m_PortraitTrans = WallpaperITrans();

    setFilename (landscapeFile, WallpaperDescriptor::Landscape);
    setFilename (portraitFile,  WallpaperDescriptor::Portrait);
    setFilename (landscapeFile, WallpaperDescriptor::OriginalLandscape);
    setFilename (portraitFile,  WallpaperDescriptor::OriginalPortrait);

    m_Valid = true;
    retval = true;

finalize:
    SYS_DEBUG ("Returning %s", SYS_BOOL(retval));
    return retval;
}

/*!
 * Method for initializing the current wallpaper descriptor from theme ids
 *
 * \param landscapeID The theme-image-id for the landscape wallpaper
 * \param portraitID The theme-image-id for the portrait wallpaper
 */
bool 
WallpaperCurrentDescriptor::setFromIDs  (
            QString     landscapeID,
            QString     portraitID)
{
    bool retval = false;
   
    SYS_DEBUG ("*** landscapeID = %s", SYS_STR(landscapeID));
    SYS_DEBUG ("*** portraitID  = %s", SYS_STR(portraitID));

    /*
     * Hack, as nowadays only portrait mode image ids are pre-set...
     */
    //if (landscapeID.isEmpty())
    //    landscapeID = portraitID;

    if (landscapeID.isEmpty () && portraitID.isEmpty ())
        goto finalize;

    if (!landscapeID.isEmpty()) {
        if (landscapeID.startsWith("/")) {
            setFilename (landscapeID, WallpaperDescriptor::Landscape);
            setFilename (landscapeID, WallpaperDescriptor::OriginalLandscape);
        } else {
            setImageID (landscapeID, WallpaperDescriptor::Landscape);
            setImageID (landscapeID, WallpaperDescriptor::OriginalLandscape);
        }
    }

    if (!portraitID.isEmpty()) {
        if (portraitID.startsWith("/")) {
            setFilename (portraitID, WallpaperDescriptor::Portrait);
            setFilename (portraitID, WallpaperDescriptor::OriginalPortrait);
        } else {
            setImageID (portraitID, WallpaperDescriptor::Portrait);
            setImageID (portraitID, WallpaperDescriptor::OriginalPortrait);
        }
    }

    m_Valid = true;
    retval = true;

finalize:
    SYS_DEBUG ("Returning %s", SYS_BOOL(retval));
    return retval;
}

/*! 
 * \returns The image transformations for the given orientation.
 * \orientation The wanted orientation
 */
WallpaperITrans 
WallpaperCurrentDescriptor::iTrans (
        M::Orientation orientation) const
{
    switch (orientation) {
        case M::Portrait:
            return m_PortraitTrans;

        case M::Landscape:
            return m_LandscapeTrans;
    }

    SYS_DEBUG ("Unknown orientation: %d", orientation);
    return m_LandscapeTrans;
}

/*!
 * \returns the original pixmap of the current wallpaper
 * \param orientation whether which orientation is wanted
 *
 * FIXME: handle the difference between the original image and the modified
 * image in the WallpaperDescriptor?
 */
QPixmap
WallpaperCurrentDescriptor::originalPixmap (
        M::Orientation orientation) const
{
    QString fileName;
    QString imageId;
    QPixmap retval;

    fileName = originalImageFile(orientation);
    imageId = imageID (orientation == M::Portrait ?
            WallpaperDescriptor::Portrait : WallpaperDescriptor::Landscape);

    if (!imageId.isEmpty()) {
        QPixmap *themedPixmap;
        
        themedPixmap = MTheme::pixmapCopy(imageId);
        retval = *themedPixmap;
        delete themedPixmap;
    } else {
        retval.load (fileName);
    }

    return retval;
}

/*!
 * \returns true if the object is properly initialized
 *
 * Currently this function will return true only if the object is initialized
 * from the desktop file. FIXME: Need to enhance to handle the gconf keys!
 */
bool
WallpaperCurrentDescriptor::valid () const
{
    return m_Valid;
}


/*!
 * Returns the filenames used when the images was last time saved. These
 * properties are stored in the desktop file.
 */
QString
WallpaperCurrentDescriptor::editedFilename (
        M::Orientation orientation) const
{
    switch (orientation) {
        case M::Landscape:
            return filename (WallpaperDescriptor::Landscape);

        case M::Portrait:
            return filename (WallpaperDescriptor::Portrait);
    }

    return QString();
}

/*!
 * Low level utility function to read data from the desktop file.
 */
bool 
WallpaperCurrentDescriptor::getValue (
        const QString  &group,
        const QString  &key, 
        QString        &value)
{
    QString  fullKey = group + "/" + key;
    bool     retval = false;

    Q_ASSERT (m_DesktopEntry);

    value = m_DesktopEntry->value (fullKey);
    if (value.isEmpty()) {
        value = "";
    } else {
        retval = true;
    }

    #ifdef LOTDEBUG
    SYS_DEBUG ("key = %s/%s value = %s", 
            SYS_STR (group), SYS_STR (key), 
            SYS_STR (value));
    #endif

    return retval;
}

/*!
 * Low level utility function to read data from the desktop file.
 */
bool 
WallpaperCurrentDescriptor::getValue (
            const QString   &group,
            WallpaperITrans &value)
{
    bool   retval;
    qreal  rval1, rval2;

    retval = getValue (group, horOffsetKey, &rval1);
    if (!retval) {
        return retval;
    }
    
    retval = getValue (group, vertOffsetKey, &rval2);
    if (!retval) {
        return retval;
    }
    value.setOffset (QPointF(rval1, rval2));
    
    retval = getValue (group, scaleKey, &rval1);
    if (!retval) {
        return retval;
    }
    value.setScale (rval1);

    return retval;
}

/*!
 * Low level utility function to read data from the desktop file.
 */
bool
WallpaperCurrentDescriptor::getValue (
            const QString &group,
            const QString &key, 
            qreal         *value)
{
    QString sValue;

    if (!getValue(group, key, sValue)) {
        *value = 0.0;
        return false;
    }

    *value = sValue.toDouble();

    #ifdef LOTDEBUG
    SYS_DEBUG ("Value for %s/%s is %g", SYS_STR(group), SYS_STR(key), *value);
    #endif
    return true;
}
