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

#include "wallpaperdescriptor.h"

#include <QUrl>
#include <QDir>
#include <QFileInfo>

#include <QPixmap>
#include <QPainter>
#include <QThread>

#include <MTheme>

#include "wallpaperthumb.h"

/*
 * Please note that printing non UTF-8 characters might break the tests.
 * Apparently the test engine can not tolerate the debug messages when there are
 * weird file names around.
 */
//#define LOTDEBUG
#define DEBUG
#define WARNING
#include "../debug.h"

static const QString dir = "";

/*
 * Good to know: In order to use this flavor under scratchbox one must install
 * the media-settings-harmattan package. Then restart the desktop...
 */
static const QString defaultFlavor = "grid";
static const int defaultThumbnailWidth = 172;
static const int defaultThumbnailHeight = 172;

/******************************************************************************
 * Image implementation.
 */
Image::Image () :
    QObject () 
{
    reset ();
}

Image::~Image ()
{
}


Image::Image (
        const Image &orig) :
    QObject ()
{
    m_Filename = orig.m_Filename;
    m_MimeType = orig.m_MimeType;
    m_Image    = orig.m_Image;
    m_ImageID  = orig.m_ImageID;
    m_Cached   = orig.m_Cached;
    m_Url      = orig.m_Url;  
    m_ThumbnailPixmap = orig.m_ThumbnailPixmap;
    m_HasThumbnail = orig.m_HasThumbnail;
}

void 
Image::reset ()
{
    m_Filename = "";
    m_MimeType = "";
    m_ImageID  = "";
    m_Cached   = false;
    m_Url      = QUrl ();

    m_ThumbnailPixmap = QPixmap();
    m_HasThumbnail = false;
}

Image &
Image::operator= (
        const Image &rhs)
{
    if (this != &rhs) {
        m_Filename = rhs.m_Filename;
        m_MimeType = rhs.m_MimeType;
        m_Image    = rhs.m_Image;
        m_ImageID  = rhs.m_ImageID;
        m_Cached   = rhs.m_Cached;
        m_Url      = rhs.m_Url;  
        m_ThumbnailPixmap = rhs.m_ThumbnailPixmap;
        m_HasThumbnail = rhs.m_HasThumbnail;
    }

    return *this;
}

void
Image::setFilename (
        const QString &fileName)
{
    SYS_DEBUG ("*** m_Filename = %s", SYS_STR(m_Filename));
    SYS_DEBUG ("*** fileName   = %s", SYS_STR(fileName));

    reset ();
    m_Filename     = fileName;
    m_Url          = QUrl::fromLocalFile (fileName);
}
    
void
Image::setMimeType (
        const QString &mimeType)
{
    m_MimeType = mimeType;
}

static const char *
mimetypes[][2] = 
{
    { "bmp",  "image/bmp"  },
    { "gif",  "image/gif"  },
    { "jpg",  "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "jpe",  "image/jpeg" },
    { "tif",  "image/tiff" },
    { "png",  "image/png"  },
    { NULL,   NULL}
};

QString
Image::mimeType () const
{
    QString retval = m_MimeType;

    /*
     * One desparate attempt to produce a mimetype for it is very important.
     * This mechanism will fire up only for the images that are not coming from
     * the Tracker, for the Tracker actually tells us the mimetype. And it does
     * not really matter, because if the thumbnail generation fails we do the
     * thumbnail ourselves.
     */
    if (retval.isEmpty() && !m_Filename.isEmpty()) {
        QString ext = extension();

        for (int n = 0; mimetypes[n][0] != NULL; ++n) {
            if (!QString::compare(ext, mimetypes[n][0], Qt::CaseInsensitive)) {
                SYS_DEBUG ("Guessing mimetype %s", SYS_STR(retval));
                retval = QString(mimetypes[n][1]);
            }
        }
    }

    return retval;
}

QString
Image::filename () const
{
    return m_Filename;
}

void
Image::setImageID (
        const QString &imageID)
{
    if (m_ImageID == imageID)
        return;

    reset ();
    m_ImageID = imageID;
}

QString
Image::imageID () const
{
    return m_ImageID;
}

void
Image::setUrl (
        const QString &urlString)
{
    QUrl     newUrl;

    newUrl.setEncodedUrl (urlString.toAscii());
    if (newUrl == m_Url)
        return;

    reset ();
    m_Filename = newUrl.path();
    m_Url = newUrl;
}


QString
Image::basename () const 
{  
    QFileInfo fileInfo (m_Filename);

    return fileInfo.baseName();
}

QString
Image::extension () const 
{
    QFileInfo fileInfo (m_Filename);

    return fileInfo.suffix();
}

/*
 * Low level loading method that will not just load the image, but also will
 * read the exim metadata and rotate the image if necessary.
 */
bool 
Image::load (
        const QString &fileName)
{
    bool retval;
    QSize     mySize;
    QSize     expectedSize;
    QuillImageFilter *loadFilter;

    QuillFile quillFile  (fileName);
    mySize = quillFile.fullImageSize ();
    /*
     * FIXME: Yeah, the size. These literals should not be here... but the code
     * changed, we are not loading the full size image any more.
     */
     mySize.scale (480, 854, Qt::KeepAspectRatio);

#if 1
    loadFilter = QuillImageFilterFactory::createImageFilter(
            QuillImageFilter::Role_Load);
    loadFilter->setOption(
            QuillImageFilter::FileName,
            QVariant(fileName));
    m_Image = loadFilter->apply (QImage(mySize, QImage::Format_RGB16));
    retval = true;
    delete loadFilter;
#else
    retval = m_Image.load(fileName);
#endif

    return retval;
}

/*!
 * \param threadSafe Not in main thread, only thread safe operations are
 *   allowed.
 */
void 
Image::cache (
        bool threadSafe)
{
    if (m_Cached) {
        return;
    } else if (filename().isEmpty() && m_ImageID.isEmpty()) {
        return;
    }
    
    SYS_DEBUG ("Caching %p", this);
    SYS_DEBUG ("*** threadSafe = %s", SYS_BOOL(threadSafe));
    SYS_DEBUG ("*** m_Cached   = %s", SYS_BOOL(m_Cached));
    SYS_DEBUG ("*** filename() = '%s'", SYS_STR(filename()));
    SYS_DEBUG ("*** m_ImageID  = '%s'", SYS_STR(m_ImageID));
    
    /*
     * Secondary thread file loading.
     */
    if (!filename().isEmpty()) {
        SYS_DEBUG ("Loading %s", SYS_STR(filename()));
        if (!load(filename())) {
            SYS_DEBUG ("Loading failed: %s", SYS_STR(filename()));
            m_Cached = false;
        } else {
            SYS_DEBUG ("Loaded %s size %dx%d", SYS_STR(filename()),
                        m_Image.width(), m_Image.height());
            m_Cached = true;
        }

        return;
    }

    /*
     * Main thread theme image loading.
     */
    if (!threadSafe && !m_ImageID.isEmpty()) {
        QPixmap       *pixmap;
        const QPixmap *cPixmap;
       
        /*
         * First we try the cached pixmap, then we force the loading. We do
         * this, because we had some problems with the theme damon when we used
         * the pixmapCopy() the second time: the application was blocked.
         */
        cPixmap = MTheme::instance()->pixmap (m_ImageID);
        if (cPixmap->width() > 1 && cPixmap->height()) {
            m_Image = cPixmap->toImage();
            m_Cached = true;
            MTheme::instance()->releasePixmap (cPixmap);
            return;
        } else {
            MTheme::instance()->releasePixmap (cPixmap);
        }


        pixmap = MTheme::instance()->pixmapCopy (m_ImageID);
        m_Image = pixmap->toImage();
        
        delete pixmap;
        m_Cached = true;
        return;
    }
}

void
Image::unCache ()
{
    SYS_DEBUG ("Uncaching...");
    if (!m_Cached)
        return;

    m_Cached = false;
    m_Image = WallPaperImage();
}

QImage &
Image::image ()
{
    cache ();
    return m_Image;
}

QImage 
Image::scaledImage (
        QSize size)
{
    if (m_Cached &&
            (m_Image.width() == size.width() ||
            m_Image.height() == size.height())) {
        SYS_DEBUG ("File scaled to this size.");
    }

    return m_Image;
}

bool
Image::setThumbnailPixmap (
        const QPixmap &pixmap)
{
    m_ThumbnailPixmap = pixmap;
    if (pixmap.height() >= 10 && pixmap.width() >= 10)
        m_HasThumbnail = true;
    else
        m_HasThumbnail = false;

    return m_HasThumbnail;
}

bool
Image::thumbnail (
        bool force)
{
    QPixmap *pixmap;
    bool     retval = false;

    if (!m_ImageID.isEmpty()) {
        /*
         * If we have an image ID instead of a filename the thumbler will not
         * help us. In this case we shall create a thumbnail using the theme.
         */
        pixmap = MTheme::instance()->pixmapCopy(m_ImageID, 
                QSize (defaultThumbnailWidth, defaultThumbnailHeight));
        m_ThumbnailPixmap = *pixmap;
        m_HasThumbnail = true;
        delete pixmap;
        retval = true;
    } else if (force) {
        /*
         * And if the force was set that means we have to make our own thumbnail
         * most probably the thumbler failed.
         */
#if 0
        // FIXME: Do we still need this?
        cache ();
        if (m_Cached) {
            SYS_DEBUG ("m_Image.scaled (");
            m_ThumbnailPixmap = 
                QPixmap::fromImage (m_Image.scaled (
                    defaultThumbnailWidth, defaultThumbnailHeight));
            m_HasThumbnail = true;
            retval = true;
        } else {
            SYS_WARNING ("No image after caching?");
        }
#endif
    }

    return retval;
}

/******************************************************************************
 * WallpaperDescriptor implementation.
 */
WallpaperDescriptor::WallpaperDescriptor(
        QObject *parent) : 
    QObject (parent),
    m_Images (NVariants),
    m_Loading (false)
{
}

WallpaperDescriptor::WallpaperDescriptor (
        const WallpaperDescriptor &orig) :
    QObject (),
    m_Images (NVariants),
    m_Loading (false)
{
    m_Images = orig.m_Images;
}

WallpaperDescriptor::WallpaperDescriptor(
        const QString &filename) : 
    QObject (),
    m_Images (NVariants),
    m_Loading (false)
{
    setFilename (filename);
}

WallpaperDescriptor::~WallpaperDescriptor()
{
#ifndef THUMBNAILER_SINGLETON
    SYS_WARNING ("m_Thumbnailer valid = %s", SYS_BOOL(m_Thumbnailer));
    if (m_Thumbnailer != 0) {
        SYS_DEBUG ("DESTROYING THUMBNAILER");
        delete m_Thumbnailer;
        m_Thumbnailer = 0;
    }
#endif
}


void
WallpaperDescriptor::setImageID (
        const QString &imageID,
        ImageVariant   variant)
{
    m_Images[variant].setImageID (imageID);
}

QString
WallpaperDescriptor::imageID (
            ImageVariant   variant) const
{
    return m_Images[variant].imageID();
}

/*!
 * \param filename The full path filename for the image file
 * \param variant The purpose of the image file
 *
 * Sets the image file name for the given purpose in the descriptor.
 */
void
WallpaperDescriptor::setFilename (
        const QString &filename,
        ImageVariant   variant)
{
    SYS_DEBUG ("*** variant = %d", variant);
    m_Images[variant].setFilename (filename);
}

QString
WallpaperDescriptor::filename (
        ImageVariant   variant) const
{
    return m_Images[variant].filename();
}

void
WallpaperDescriptor::setUrl (
        const QString &urlString,
        ImageVariant   variant)
{
    m_Images[variant].setUrl (urlString);
}

QString
WallpaperDescriptor::basename (
        ImageVariant   variant) const
{
    return m_Images[variant].basename();
}

/*!
 * \returns The filename extension of the filename. 
 *
 * The filename extension is used when the applet creates a new file to save the
 * edited image. The edited file will use the same extension used in the
 * original filename.
 */
QString
WallpaperDescriptor::extension ( 
        ImageVariant   variant) const
{
    return m_Images[variant].extension ();
}


QPixmap
WallpaperDescriptor::thumbnailPixmap() const 
{
    if (m_Images[WallpaperDescriptor::Portrait].hasThumbnail())
        return m_Images[WallpaperDescriptor::Portrait].thumbnailPixmap();

    if (m_Images[WallpaperDescriptor::Landscape].hasThumbnail())
        return m_Images[WallpaperDescriptor::Landscape].thumbnailPixmap();

    return m_Images[WallpaperDescriptor::Portrait].thumbnailPixmap();
}

bool 
WallpaperDescriptor::isThumbnailLoaded (
        ImageVariant   variant) const
{
    return m_Images[variant].hasThumbnail ();
}

void 
WallpaperDescriptor::cache (
    ImageVariant   variant)
{
    m_Images[variant].cache ();

    SYS_DEBUG ("================================>");
    for (int n = 0; n < m_Images.size(); ++n) {
        SYS_DEBUG ("*** n = %d", n);
        SYS_DEBUG ("*** thisfilename  = %s", SYS_STR(m_Images[n].filename()));
        SYS_DEBUG ("*** otherfilename = %s", SYS_STR(m_Images[variant].filename()));
        if (n == variant)
            continue;

        if (!m_Images[n].filename().isEmpty() &&
                    m_Images[n].filename() == m_Images[variant].filename()) {
            if (m_Images[variant].m_Cached) {
                m_Images[n].m_Image =  m_Images[variant].m_Image;
            }
        }
    }
    
    SYS_DEBUG ("<================================");
}

void
WallpaperDescriptor::unCache (
        ImageVariant   variant)
{
    SYS_DEBUG ("Uncaching %d...", variant);
    m_Images[variant].unCache ();
}

void
WallpaperDescriptor::unCache ()
{
    for (int n = Landscape; n < NVariants; ++n) {
        m_Images[n].unCache ();
    }
}

bool
WallpaperDescriptor::loading () const
{
    return m_Loading;
}

void
WallpaperDescriptor::setLoading (
        bool loading)
{
    if (loading == m_Loading)
        return;

    SYS_DEBUG ("setting m_Loading to %s", SYS_BOOL(loading));
    m_Loading = loading;
    emit changed (this);
}

QImage & 
WallpaperDescriptor::image (
            ImageVariant   variant)
{
    return m_Images[variant].image();
}
#if 0
/*!
 * Will cache the pixmap, then returns the loaded QPixmap.
 */
QImage 
WallpaperDescriptor::scaledImage (
        QSize          size,
        ImageVariant   variant)
{
    SYS_DEBUG ("------------------------------------------------");
    SYS_DEBUG ("*** variant = %d", variant);
    cache (variant);
    return m_Images[variant].scaledImage(size);
}
#endif

void
WallpaperDescriptor::setMimeType (
        const QString &mimeType,
        ImageVariant   variant)
{
    m_Images[variant].setMimeType (mimeType);
}

QString
WallpaperDescriptor::mimeType (
        ImageVariant   variant) const
{
    return m_Images[variant].mimeType ();
}

/*!
 * This function will initiate the thumbnail generation. The thumbnail will be
 * loaded into the pixmap that is returned by the thumbnailPixmap() function. I
 * guess the pixmap will notify the 
 */
void 
WallpaperDescriptor::initiateThumbnailer ()
{
    QList<QUrl>      urisList;
    QStringList      mimeList;

    SYS_DEBUG ("");
    
    if (!qApp) {
        SYS_WARNING ("QApplication must be created before calling this method");
    } else {
        bool notCurrentThread;

        notCurrentThread = qApp->thread() != QThread::currentThread();
        if (notCurrentThread) {
            SYS_WARNING ("This is not the current thread!");
        }
    }

    /*
     * If the thumbnailer is already initiated we return.
     *
     * We're asking the m_Images already in the next for cycle.
     * So no need for this pointer checking (as it is not working with
     * one Thumbnailer instance properly...)
     */
#ifndef THUMBNAILER_SINGLETON
    if (m_Thumbnailer != 0) {
        SYS_WARNING ("Thumbnailer already initiated");
        return;
    }
#endif

    for (int n = Landscape; n < NVariants; ++n) {
        if (m_Images[n].hasThumbnail())
            continue;

        if (m_Images[n].thumbnail()) {
            emit thumbnailLoaded (this);
            emit changed (this);

            continue;
        }

        if (m_Images[n].mimeType().isEmpty() ||
                m_Images[n].filename().isEmpty()) {
            continue;
        }
    
        urisList << m_Images[n].url();
        mimeList << m_Images[n].mimeType();
    }

    if (urisList.size() == 0)
        return;

#ifdef THUMBNAILER_SINGLETON
    m_Thumbnailer = WallpaperThumb::getInstance ()->thumbnailer ();
#else
    m_Thumbnailer = new Thumbnailer;
#endif

    #ifdef LOTDEBUG
    QStringList list = Thumbnailer::getFlavors();
    foreach (QString flavor, list) {
        SYS_DEBUG ("*** flavor = %s", SYS_STR(flavor));
    }
    #endif
    connect (m_Thumbnailer, SIGNAL(thumbnail(QUrl,QUrl,QPixmap,QString)),
            this, SLOT(thumbnailReady(QUrl,QUrl,QPixmap,QString)) );
    connect (m_Thumbnailer, SIGNAL(error(QString,QUrl)),
            this, SLOT(thumbnailError(QString,QUrl)) );
#ifndef THUMBNAILER_SINGLETON
    connect (m_Thumbnailer, SIGNAL(finished(int)),
            this, SLOT(thumbnailLoadingFinished(int)));
#endif

    m_Thumbnailer->request (urisList, mimeList, true, defaultFlavor);
}

/*!
 * This slot is called when the thumbnail has been generated.
 */
void 
WallpaperDescriptor::thumbnailReady (
            QUrl         fileUri, 
            QUrl         thumbnailUri, 
            QPixmap      pixmap, 
            QString      flavor)
{
    bool success;

    Q_UNUSED (thumbnailUri);
    Q_UNUSED (flavor);

#ifdef LOTDEBUG
    SYS_DEBUG ("*** flavor = %s", SYS_STR(flavor));
    SYS_DEBUG ("*** size   = %dx%d", pixmap.width(), pixmap.height());
#endif
    /*
     * FIXME: should store the thumbnail URL as well.
     */
    for (int n = Landscape; n < NVariants; ++n) {
        if (m_Images[n].url() == fileUri) {
            //SYS_DEBUG ("Has thumbnail for %dth image", n);
            success = m_Images[n].setThumbnailPixmap (pixmap);
        }
    }

    emit thumbnailLoaded (this);
    emit changed (this);
}

/*!
 * This slot is called when the thumbler has been detected some error. We could
 * do something, but until we find some smart thing we can do we just print an
 * error message.
 */
void
WallpaperDescriptor::thumbnailError (
            QString      message,
            QUrl         fileUri)
{
    Q_UNUSED (message);

    SYS_WARNING ("*** message = %s", SYS_STR(message));
    for (int n = Landscape; n < NVariants; ++n) {
        if (m_Images[n].url() == fileUri) {
            bool success;

            SYS_WARNING ("Failed thumbnailing for %dth image: %s", 
                    n, SYS_STR(message));
            success = m_Images[n].thumbnail (true);

            if (success) {
                emit thumbnailLoaded (this);
                emit changed (this);
            }
        }
    }
}

/*!
 * Called when the generation of the thumbnail is finished. This is the point
 * where we can destroy the thumbler object.
 */
void 
WallpaperDescriptor::thumbnailLoadingFinished (
            int          left)
{
    Q_UNUSED (left);

#ifndef THUMBNAILER_SINGLETON
    if (!m_Thumbnailer.isNull() && left == 0) {
        SYS_WARNING ("DESTROYING THUMBNAILER");
        delete m_Thumbnailer;
        m_Thumbnailer = 0;
    }
#endif
}

/*!
 * \returns true if this is the current wallpaper.
 */
bool
WallpaperDescriptor::isCurrent () const
{
    return false;
}

/*!
 * Applications need to listen the wallpaper GConf items, so whenever we save a
 * file we need to modify the filename of the saved images. We could re-use the
 * filename, but then the GConf key would not be changed, so the applications
 * would not be able to recognize the change.
 * 
 * For this purpose we use a version number in the filenames. The
 * WallpaperDescriptor returns one default value for version number, the
 * WallpaperCurrentDescriptor on the other hand reads the version number from
 * the desktop file. 
 */
int
WallpaperDescriptor::version () const 
{
    return 0;
}

/*
 * \param orientation Selects which output file we are talking about.
 * \param ver The version number or -1 to use the objects version number.
 * Returns a suggested file name that is used when a modified image file is
 * saved.
 */

// Quick fix: it seems that the jpeg save has some bugs. See NB#182044 and
// NB##180067 for details.
#define ALWAYS_PNG

QString 
WallpaperDescriptor::suggestedOutputFilename (
        M::Orientation orientation,
        int            ver) const
{
    QString retval;
#ifndef ALWAYS_PNG
    QString outputExtension;
#endif
    QString myBasename;
    QString myExtension;

    if (ver < 0)
        ver = version ();


    switch (orientation) {
        case M::Landscape:
#ifndef ALWAYS_PNG
            outputExtension = extension(WallpaperDescriptor::OriginalLandscape);
            if (outputExtension.isEmpty())
                outputExtension = extension(WallpaperDescriptor::Landscape);
#endif

            myBasename = basename (WallpaperDescriptor::OriginalLandscape);
            if (myBasename.isEmpty())
                myBasename = basename (WallpaperDescriptor::Landscape);

            myExtension = "-landscape.";
            break;

        case M::Portrait:
#ifndef ALWAYS_PNG
            outputExtension = extension(WallpaperDescriptor::OriginalPortrait);
            if (outputExtension.isEmpty())
                outputExtension = extension(WallpaperDescriptor::Portrait);
#endif

            myBasename = basename (WallpaperDescriptor::OriginalPortrait);
            if (myBasename.isEmpty())
                myBasename = basename (WallpaperDescriptor::Portrait);

            myExtension = "-portrait.";
            break;
    }

    /*
     * There are some extensions we can not support for writing.
     */
#ifndef ALWAYS_PNG
    if (outputExtension.isEmpty() ||
        QString::compare(outputExtension, "gif", Qt::CaseInsensitive) == 0 ||
        QString::compare(outputExtension, "bpm", Qt::CaseInsensitive) == 0 ||
        QString::compare(outputExtension, "pgm", Qt::CaseInsensitive) == 0)
        outputExtension = "png";
#endif

    retval = myBasename + myExtension + 
        QString::number(ver) + "." + 
#ifndef ALWAYS_PNG
        outputExtension;
#else
        "png";
#endif

    return retval;
}

QString 
WallpaperDescriptor::suggestedOutputMimeType (
            M::Orientation orientation)
{
    QString retval;

    switch (orientation) {
        case M::Landscape:
            retval = mimeType(WallpaperDescriptor::OriginalLandscape);
            if (retval.isEmpty())
                retval = mimeType (WallpaperDescriptor::Landscape);
            break;

        case M::Portrait:
            retval = mimeType (WallpaperDescriptor::OriginalPortrait);
            if (retval.isEmpty())
                retval = mimeType (WallpaperDescriptor::Portrait);
            break;
    }

    return retval;
}

/*
 * Returns the original image file that was loaded, modified and saved into a
 * new file. For this class the original filename is the only filename.
 *
 * FIXME: What if the original file is deleted?
 */
QString 
WallpaperDescriptor::originalImageFile (
        M::Orientation orientation) const
{
    QString retval;

    switch (orientation) {
        case M::Portrait:
            retval = filename (WallpaperDescriptor::OriginalPortrait);
            if (retval.isEmpty())
                retval = filename (WallpaperDescriptor::Portrait);

            return retval;

        case M::Landscape:
            retval = filename (WallpaperDescriptor::OriginalLandscape);
            if (retval.isEmpty())
                retval = filename (WallpaperDescriptor::Landscape);

            return retval;
    }

    SYS_WARNING ("Unknown orientation: %d", orientation);
    return filename (WallpaperDescriptor::OriginalLandscape);
}

/*
 * This function always return true, only the inherited
 * WallpaperCurrentDescriptor has tests in it.
 */
bool
WallpaperDescriptor::valid () const
{
    return true;
}

/*!
 * This method might be called from the non-GUI thread. In that case the method
 * will load the image files as QImage objects, because the QImage can be
 * handled from the secondary threads while QPixmap not.
 */
void
WallpaperDescriptor::loadAll () 
{
    #if 0
    QSize landscapeSize (854, 480);
    #endif
    QSize portraitSize (480, 854);
    bool  threadSafe = false;

    /*
     * Need to find out if we are in the GUI thread or in some secondary thread
     * created so that we can stay responsive while the images are loaded.
     */
    if (!qApp) {
        SYS_WARNING ("QApplication must be created before calling this method");
    } else {
        threadSafe = qApp->thread() != QThread::currentThread();
    }

    /*
     * We go through the images and load them: FIXME: how could we optimize this
     * and load only those images that are going to be used?
     */
    SYS_DEBUG ("secondary thread = %s", SYS_BOOL(threadSafe));
    for (int n = 0; n < m_Images.size(); ++n) {
        bool alreadyFound = false;
        SYS_DEBUG ("*************** %d ***", n);

        if (m_Images[n].m_Cached)
            continue;

        for (int i = 0; i <  m_Images.size(); ++i) {
            SYS_DEBUG ("images[%d] file %s; images[%d] file %s",
                    n, SYS_STR(m_Images[n].filename()),
                    i, SYS_STR(m_Images[i].filename()));
            if (!m_Images[n].filename().isEmpty() &&
                    m_Images[n].filename() == m_Images[i].filename()) {
                alreadyFound = true;
                SYS_DEBUG ("Not loading %d, it is the same as %d.", n, i);
                if (m_Images[i].m_Cached) {
                    m_Images[n].m_Image =  m_Images[i].m_Image;
                }
                break;
            }
        }
        
        if (alreadyFound) {
            continue;
        }

        SYS_DEBUG ("caching %d", n);
        m_Images[n].cache (threadSafe);
    }

    /*
     * Not just the loading but the scaling is also blocking the GUI thread.
     * Here we try to find out what size we are about to use and we do some
     * thread safe pre-scaling.
     */
#if 0
    m_Images[WallpaperDescriptor::Portrait].preScale (
            portraitSize, threadSafe);
    m_Images[WallpaperDescriptor::OriginalPortrait].preScale (
            portraitSize, threadSafe);
#endif
}

