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
#include "ft_profiledatainterface.h"
#include "profiledatainterface.h"

#include <MApplication>

#define DEBUG
#include "../../src/debug.h"

int tracker_init_calls = 0;
int tracker_quit_calls = 0;

#ifdef HAVE_LIBPROFILE
#include <libprofile.h>

extern "C" {

int
profile_tracker_init ()
{
    ++tracker_init_calls;
    return 0;
}

void
profile_tracker_quit ()
{
    ++tracker_quit_calls;
}

} // extern "C"
#endif

void 
Ft_ProfileDataInterface::init()
{
}

void 
Ft_ProfileDataInterface::cleanup()
{
}

static int argc = 1;
static char* app_name = (char*) "./ft_profiledatainterface";

void 
Ft_ProfileDataInterface::initTestCase()
{

    m_App = new MApplication (argc, &app_name);
}

void 
Ft_ProfileDataInterface::cleanupTestCase()
{
    delete m_App;
}

void 
Ft_ProfileDataInterface::testStatusMenuRegression ()
{
#ifdef HAVE_LIBPROFILE
    ProfileDataInterface    *dataIf1;
    ProfileDataInterface    *dataIf2;

    dataIf1 = new ProfileDataInterface;
    dataIf2 = new ProfileDataInterface;

    // profile_tracker_init should be called only once
    QCOMPARE (tracker_init_calls, 1);

    delete dataIf2;
    dataIf2 = 0;

    // profile tracker still should be alive 
    QCOMPARE (tracker_quit_calls, 0);

    delete dataIf1;
    dataIf1 = 0;
#endif
}

QTEST_APPLESS_MAIN(Ft_ProfileDataInterface)
