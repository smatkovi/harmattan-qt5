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
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
** Authors: David Kedves <dkedves@blumsoft.eu>
**          Laszlo Pere <lpere@blumsoft.eu>
**
****************************************************************************/
#include "wallpaperworkerthread.h"
#include "wallpaperdescriptor.h"
#include <QFile>

#define DEBUG
#define WARNING
#include "../debug.h"

WallpaperWorkerThread::WallpaperWorkerThread (
        QPixmap         &pixmap, 
        const QString   &originalFileName,
        const QString   &outputFileName,
        const QSize     &size) :
    m_Task (TaskSaveImage),
    m_OriginalFileName (originalFileName),
    m_OutputFileName (outputFileName),
    m_Size (size),
    m_Success (false)
{
    m_Image = pixmap.toImage();
}

WallpaperWorkerThread::WallpaperWorkerThread (
        const QString   &originalFileName,
        const QString   &outputFileName) :
    m_Task (TaskCopyFile),
    m_OriginalFileName (originalFileName),
    m_OutputFileName (outputFileName)
{
}

WallpaperWorkerThread::WallpaperWorkerThread (
        WallpaperDescriptor  &desc,
        const QSize          &size) :
    m_Task (TaskLoadImage),
    m_Descriptor (desc),
    m_Size (size)
{
}

void
WallpaperWorkerThread::run ()
{
    SYS_DEBUG ("*** m_Image          = %dx%d", 
            m_Image.width(), m_Image.height());
    SYS_DEBUG ("*** m_OutputFileName = %s", SYS_STR(m_OutputFileName));

    switch (m_Task) {
        case TaskSaveImage:
            runSaveImage ();
            break;

        case TaskLoadImage:
            runLoadImage ();
            break;

        case TaskCopyFile:
            runCopyFile ();
            break;
    }

    SYS_DEBUG ("End of thread");
}

void
WallpaperWorkerThread::runSaveImage ()
{
    QFile    targetFile (m_OutputFileName);

    if (targetFile.exists()) {
        m_Success = targetFile.remove ();
        if (!m_Success) {
                SYS_WARNING ("ERROR: removing '%s': %m", 
                        SYS_STR(m_OutputFileName));
                return;
        }
    }

    if (m_Image.size() != m_Size) {
        SYS_DEBUG ("Size mismatch...");
        m_Success = m_Image.scaled (m_Size).save (m_OutputFileName);
    } else {
        m_Success = m_Image.save (m_OutputFileName);
    }
}

void
WallpaperWorkerThread::runLoadImage ()
{
    QSize originalSize;

    m_Image = m_Descriptor.load (m_Size, originalSize);
    m_Size = originalSize;

    if (m_Image.width() == 0 || m_Image.height() == 0)
        m_Success = false;
    else
        m_Success = true;
}

void
WallpaperWorkerThread::runCopyFile ()
{
    QFile    origFile (m_OriginalFileName);

    m_Success = origFile.copy(m_OutputFileName);
    if (m_Success) {
        goto finalize;
    } else {
        SYS_WARNING ("WARNING: can't copy file to %s", 
                SYS_STR(m_OutputFileName));
        QFile targetFile (m_OutputFileName);
        if (targetFile.exists()) {
            SYS_WARNING ("WARNING: File exists, trying to overwrite.");
            m_Success = targetFile.remove ();

            if (!m_Success) {
                SYS_WARNING ("ERROR: removing '%s': %m", 
                        SYS_STR(m_OutputFileName));
                goto finalize;
            }
        }
    }
    
    m_Success = origFile.copy (m_OutputFileName);

finalize:
    if (!m_Success) {
        SYS_WARNING ("ERROR copying file: %m");
    }
}

bool 
WallpaperWorkerThread::success () const
{
    return m_Success;
}

QString 
WallpaperWorkerThread::originalFileName() const
{
    return m_OriginalFileName;
}

QString 
WallpaperWorkerThread::outputFileName () const
{
    return m_OutputFileName;
}

QuillImage
WallpaperWorkerThread::image () const
{
    return m_Image;
}

QSize
WallpaperWorkerThread::size () const
{
    return m_Size;
}
