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

#include <QObject>

#include <QPointer>

#ifdef HAVE_GALLERYCORE
#include <QUrl>
#include <QImage>
#include <gallerymodel.h>
#include <gallerygridpage.h>
#include <galleryfullscreenpage.h>
#include "wallpaperimagecontentprovider.h"
#endif

class QString;
class MGConfItem;
class WallpaperDescriptor;
class QStringList;
class WallpaperITrans;
class WallpaperWorkerThread;

#include <QPointer>
#include <QuillImage>
#include <QSize>
#include <QTimer>

#include <wallpaperdescriptor.h>

/*!
 * The logic which handling the loading/saving and management work for 
 * wallpaper settings applet

 A little help abou the wallpaper GConf settings. Here is how the GConf database
 should look like:

 /desktop/meego/background/portrait:
  history = [/home/pipas/MyDocs/.wallpapers/100_0052.JPG,/home/pipas/MyDocs/.wallpapers/100_0051.JPG,/home/pipas/MyDocs/.wallpapers/100_0041.JPG,/home/pipas/MyDocs/.wallpapers/DSC05181.JPG,/home/pipas/MyDocs/.wallpapers/100_0037.JPG,/home/pipas/MyDocs/.wallpapers/DSCF2029.JPG,/home/pipas/MyDocs/.wallpapers/100_0039.JPG,/home/pipas/MyDocs/.wallpapers/100_0044.JPG,/home/pipas/MyDocs/.wallpapers/100_0038.JPG,/home/pipas/MyDocs/.wallpapers/100_0043.JPG]
  picture_filename = /home/pipas/.wallpapers/100_0052.png
  original_filename = [/home/pipas/.wallpapers/100_0052.png,/home/pipas/MyDocs/.wallpapers/100_0052.JPG]

 (1) /desktop/meego/background/portrait/picture_filename

     The full path of the current portrait wallpaper image or the logical name
     of the theme image if a theme file is used. The path always starts with 
     '/', the logical ID is never.

     This value is used by any package that needs to get the portrait wallpaper 
     image.

 (2) /desktop/meego/background/portrait/original_filename

     A string list that contains exactly two values, the current portrait
     wallpaper image path (the same as in 1.) and the original image path that
     was used as image source when the wallpaper image was set.

     The wallpaper applet uses this value to find the original image file, so
     that the list view of the wallpaper applet will never show both the
     original image file and the current value. The current wallpaper image is
     usually a slightly modified (or not modified at all) version of some
     "original file" and apparently showing both images is a very bad user 
     experience.

     This GConf key holds both the original and the current value and if some
     application is setting only the current wallpaper key (as in 1.) the
     wallpaper applet will ignore the original image value. This is an important
     protection.

     All application that sets the portrait wallpaper should also set this
     value.

 (3) /desktop/meego/background/portrait/history

     The wallpaper applet shows the available wallpapers in the order of the
     "history", a backward timeline of modifications. This list contains the
     last 10 wallpapers, the most recently used (current) at first, then the
     previous wallpaper, and so on.

     The applications that change the wallpaper should always add the original 
     file path to the beginning of the list (or move this value to the first
     place if it is already in the list) and truncate the list so that it
     contains 10 items at max.
 */
class MC_EXPORT WallpaperBusinessLogic : public QObject
{
    Q_OBJECT

    public:
        WallpaperBusinessLogic (QObject *parent = 0);
        ~WallpaperBusinessLogic ();

        void currentWallpaper (
                QString   &currentFilePath,
                QString   &originalFilePath);

        QStringList history () const;

        void startEdit (WallpaperDescriptor desc);
        void endEdit ();
        bool setWallpaper ();
        bool setWallpaper (QPixmap &pixmap);
        bool loadImage (WallpaperDescriptor &desc);
        
        WallpaperDescriptor editedImage () const;
        bool hasEditedImage () const;

        QSize sceneSize () const;
        
    signals:
        void wallpaperChanged ();
        void wallpaperSaved ();
        void workerStarted ();
        void workerEnded ();
        void wallpaperLoaded (QuillImage image, QSize originalSize);
        void editWallpaper (WallpaperDescriptor desc);

    public slots:
        #ifdef HAVE_GALLERYCORE
        void galleryActivated ();

        void onGridPageItemSelected (QUrl url);
        void onGridPageCancelled ();

        void croppingDone (QImage croppedImage);
        void croppingCancelled ();
        void setProgressIndicator (bool busy);
        #endif 

    private slots:
        void portraitGConfChanged ();
        void portraitHistoryChanged ();
        void workerThreadFinishedLoad ();
        void workerThreadFinishedSave ();
        void gconfTimerSlot ();
        #if 0
        bool supportsLandscape () const;
        bool supportsPortrait () const;
        #endif

    private:
        void prependFileToHistory (const QString &path);
        void prepareGalleryWidgets ();

    private:
        bool                       m_EditRequested;
        QPointer<MGConfItem>       m_PPItem;
        QPointer<MGConfItem>       m_POItem;
        QPointer<MGConfItem>       m_PHItem;
        WallpaperDescriptor        m_EditedImage;
        WallpaperWorkerThread     *m_WorkerThread;
        QTimer                     m_GConfTimer;
        QStringList                m_PortraitHistory;
#ifdef HAVE_GALLERYCORE
        QPointer<GalleryFullScreenPage>   m_FullScreenPage;
        QPointer<GalleryGridPage>         m_GalleryGridPage;
        QPointer<GalleryModel>            m_GalleryModel;
        QPointer<ImageContentProvider>    m_ImageContentProvider;
#endif

#ifdef UNIT_TEST
    friend class Ut_WallpaperBusinessLogic;
    friend class Ft_WallpaperBusinessLogic;
#endif
};

#endif
