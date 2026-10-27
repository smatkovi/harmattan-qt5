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
#ifndef WALLPAPERBUSINESSLOGIC_H
#define WALLPAPERBUSINESSLOGIC_H

#include <QList>
#include <QObject>
#include <QPointer>
#include <QFileSystemWatcher>
#include <meegocontrolexport.h>

class QString;
class MGConfItem;
class WallpaperDescriptor;
class QStringList;
class WallpaperITrans;

#include <QPointer>
#include <QtConcurrentRun>
#include <QFutureWatcher>

#include <wallpaperdescriptor.h>

#define WALLPAPER_DBUS_INTERFACE "com.nokia.wallpaper"
#define WALLPAPER_DBUS_EDIT_SIGNAL "edit"

/*
             Initiating the wallpaper editor screen from DBus

 In order to initiate the wallpaper editor from an external application two
 steps has to be performed.

 (1) First the controlpanel has to be started and the wallpaper applet must be
 activated through dbus. Please check the documentation of the
 libduicontrolpanel about this step.

 (2) Then the wallpaper applet can be contacted through DBus and the editor has
 to be initiated giving two image files as parameter. The first image file is
 the portrait, while the second is the landscape version of the wallpaper that
 should be editoed. The two images might be the same, in this case the very same
 image will be cropped differently for getting the portrait and the landscape
 variants. The following interface can be used for this step:

 dbus-send --session --type=signal / com.nokia.wallpaper.edit \
    string:/usr/share/themes/base/meegotouch/images/meegotouch-wallpaper-portrait.jpg \
    string:/usr/share/themes/base/meegotouch/images/meegotouch-wallpaper-landscape.jpg
*/ 

/*!
 * The logic which handling the loading/saving and management
 * work for wallpaper settings applet
 */
class MC_EXPORT WallpaperBusinessLogic : public QObject
{
    Q_OBJECT

public:
    WallpaperBusinessLogic (QObject *parent = 0);
    ~WallpaperBusinessLogic ();

    QList<WallpaperDescriptor *> availableWallpapers () const;

    void setEditedImage (WallpaperDescriptor *desc, bool ours = false);
    void startEdit (WallpaperDescriptor *desc = NULL);

    WallpaperDescriptor *editedImage ();

    void setBackground (
        WallpaperITrans     *landscapeITrans,
        WallpaperITrans     *portraitITrans,
        WallpaperDescriptor *desc = 0);

signals:
    void wallpaperChanged ();
    void imageEditRequested ();
    void fileListChanged ();
    
private slots:
    void editRequestArrived (
        QString   portraitFileName,
        QString   landscapeFileName);

    void startEditThreadEnded ();
    void valueChanged ();
    void directoryChangedDelayed ();
    void directoryChanged (const QString &path);
    void fileChanged (const QString &path);

private:
    typedef enum {
        SaveDir,
        DownloadDir,
        MountDir,
        SystemDir,
        MeegoDir
    } WallpaperDirectoryID;

    QString dirPath (WallpaperDirectoryID dirID = SaveDir) const;
    void startWatchingFiles ();
    bool ensureHasDirectory ();
    void createBackupFiles ();
    void deleteBackupFiles ();
    void saveOriginal (
        WallpaperDescriptor *desc);
    bool writeFiles (
        WallpaperITrans     *landscapeITrans,
        WallpaperITrans     *portraitITrans,
        WallpaperDescriptor *desc);

    void makeBackup (const QString &filePath);
    void makeImageFile (
            const QString        &filePath,
            WallpaperDescriptor  *desc,
            WallpaperITrans      *transformations);

    bool supportsLandscape () const;
    bool supportsPortrait () const;

private:
    MGConfItem                    *m_LandscapeGConfItem;
    MGConfItem                    *m_PortraitGConfItem;
    QPointer<WallpaperDescriptor>  m_EditedImage;
    bool                           m_EditedImageOurs;
    bool                           m_OrientationLocked;
    M::Orientation                 m_LockedOrientation;
    QFutureWatcher <void>          m_FutureWatcher;
    QPointer<QFileSystemWatcher>   m_FileWatcher;
#ifdef UNIT_TEST
    friend class Ut_WallpaperBusinessLogic;
    friend class Ft_WallpaperBusinessLogic;
#endif
};

#endif
