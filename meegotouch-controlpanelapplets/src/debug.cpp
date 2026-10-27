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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <QString>
#include <QDir>

#include "debug.h"

/*! \mainpage libMeegoControl 
 *
 * \section intro_sec Introduction
 * The libMeegoControl library is a very simple library created using the Meego
 * Control Panel Applets as base source. The primary repository for the Meego
 * Control Panel Applets can be found at the following URL.
\verbatim
http://meego.gitorious.org/meegotouch/meegotouch-controlpanelapplets
\endverbatim
 *
 * The code that is re-used for this library is designed to operate in the Meego
 * Controlpanel as a GUI applet, it is not intended to be used as a standalone 
 * library. Taking part of the applet code and using it outside its normal
 * operational environment is highly experimental and while it is stable, it
 * might be not as efficient as it could be. 
 *
 * Currently the libMeegoControl library holds only a few classes from the
 * controlpanel applets to support the setting of the wallpaper, the background
 * image for the meego desktop and the meego lockscreen. More functionality
 * might be added later.
 *
 * If you have any problems using the library please file a bug at
 * https://projects.maemo.org/bugzilla/ in the H-Setting->wallpaperapplet
 * component.
 *
 * \section using Using the library
 * To use the library first it has to be added to the configuration of the
 * application project file:
\code
CONFIG += \
    meegocontrol \
    silent
\endcode
 *  
 * To set the wallpaper one must create a WallpaperDescriptor that holds the
 * information about a set of images (usually one image for landscape and one
 * more image for portrait orientation), two objects of type WallpaperITrans
 * that describes the image transformations for the two orientation and finally
 * a WallpaperBusinessLogic object that does the job.
 *
 * Here is a small example that sets the wallpaper using two image files. If the
 * same image file is intended for both orientations, the same filename can be
 * passed twice.
\code
#include <meegocontrol.h>

int
main (int argc, char** argv)
{
        QApplication           app (argc, argv);
        WallpaperBusinessLogic *logic;
        WallpaperDescriptor    *desc;
        WallpaperITrans         landscapeTrans;
        WallpaperITrans         portraitTrans;
        QString                 file1;
        QString                 file2;
    
    // Checking the command line arguments.
    if (!argv[1] || !argv[2]) {
        fprintf (stderr, "Usage: \n  %s <imagefile1> <imagefile2>\n", argv[0]);
        exit (1);
    }
    
    file1 = argv[1];
    file2 = argv[2];
   
    // Initializing.
    logic = new WallpaperBusinessLogic;
    desc = new WallpaperDescriptor;

    // Setting the wallpaper here.
    desc->setFilename (file1, WallpaperDescriptor::Landscape);
    desc->setFilename (file2, WallpaperDescriptor::Portrait);
    logic->setBackground (&landscapeTrans, &portraitTrans, desc);

    // Releasing resources.
    delete desc;
    delete logic;
    return 0;
}
\endcode
 *
 * Once the wallpaper set the files are saved into the ~/.wallpapers directory
 * of the user's home and the actual settings are changed in the GConf database.
 */

static FILE *debugStream = NULL;

static inline void 
openDebugFile () 
{
    if (!debugStream) {
        QString fileName;

        fileName = QDir::homePath() + "/controlpanel-applets.log";
        printf("---> %s", SYS_STR(fileName));
        debugStream = fopen (fileName.toAscii().constData(), "a");
        if (debugStream) {
            fprintf (debugStream, 
                    "***********************************************\n"
                    "*** Debug log file opened. ********************\n"
                    "***********************************************\n");
        }
    }
}

void
SysDebug::closeDebugFile ()
{
#ifndef UNIT_TEST
    if (! debugStream)
        return;

    fflush (debugStream);
    fclose (debugStream);
    debugStream = NULL;
#endif
}

/*
 * This function is used to print debug and error messages, an enhanced version
 * of the SysDebug::sysMsg(). Please use this function anly through macros (like
 * SYS_DEBUG() for example) so the change in the function interface does not
 * mean the change of all the code calling it).
 */
void 
SysDebug::sysPrintMsg (
        QtMsgType     type,
        const char   *function,
        const char   *formatstring,
        ...)
{
    FILE *stream;
    bool  useColors;

    #ifdef DEBUG_TO_FILE
    openDebugFile ();
    #endif

    if (debugStream != NULL) {
        stream    = debugStream;
        useColors = false;
    } else {
        stream    =  stdout;
        useColors =  true;
    }

    va_list args;
    va_start (args, formatstring);
    switch (type) {
        case QtDebugMsg:
            fprintf (stream, "%s%s%s: ",
                    TERM_GREEN TERM_BOLD, function, TERM_NORMAL);
            vfprintf (stream, formatstring, args);
            break;

        case QtWarningMsg:
            fprintf (stream, "%s%s%s: ",
                    TERM_YELLOW, function, TERM_NORMAL);
            vfprintf (stream, formatstring, args);
            break;

        case QtCriticalMsg:
            fprintf (stream, "%s%s%s: ",
                    TERM_RED, function, TERM_NORMAL);
            vfprintf (stream, formatstring, args);
            break;

        case QtFatalMsg:
            fprintf (stream, "%s%s%s: ",
                    TERM_GREEN, function, TERM_NORMAL);
            vfprintf (stream, formatstring, args);
            fprintf (stream, "\n%s%s%s: Aborting program.",
                    TERM_RED TERM_BOLD, function, TERM_NORMAL);
            putchar('\n');
            fflush (stream);
            abort();
    }
    va_end (args);

    fprintf(stream, "\n");
    fflush (stream);
}

