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
#ifndef __QDIRSTUB_FOR_THEME__
#define __QDIRSTUB_FOR_THEME__

#include "mdesktopentry.h"

QStringList 
entryList (
        const QDir &dir, 
        QDir::Filters filters) 
{
    MTheme       *theme = MTheme::instance();
    QString       themeName = theme->currentTheme();
    QStringList   retval;

    Q_UNUSED (dir);
    Q_UNUSED (filters);
    //SYS_DEBUG ("*** %s", SYS_STR(dir.absolutePath()));
    if ((filters & QDir::NoDotAndDotDot) == 0)
        retval << 
	        // These are for testing if the filtering works in the
	        // ThemeBusinessLogic class.
    	    "." <<
	        "..";

    retval << 
	    // These are the fake theme names from the mdesktopentrystub 
	    // test database.
	    CODENAMEDesktopFileMissing <<
	    CODENAMEDesktopFilePerfect <<
	    CODENAMEDesktopFileHidden <<
	    CODENAMEDesktopFileEmpty <<
	    // And this is the current theme name. The mdesktopentrystub will
	    // handle this, it also reads the theme name from Meego.
	    themeName;

    #if 0
    int           n;
    n = 0;
    foreach (QString debugString, retval) {
        SYS_DEBUG ("retval[%d] = %s", n, SYS_STR(debugString));
	++n;
    }
    #endif

    return retval;
}
#endif
