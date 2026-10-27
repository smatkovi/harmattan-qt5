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
#include "ut_alerttonetoplevel.h"

#include <MAction>
#include <MApplication>
#include <QString>
#include <QStringList>
#include "alerttonetoplevel.h"
#ifdef HAVE_LIBPROFILE
#include <profiled/libprofile.h>
#endif
#include <QDebug>

/******************************************************************************
 * Ut_AlertToneToplevel implementation.
 */
void
Ut_AlertToneToplevelTests::init()
{
}

void
Ut_AlertToneToplevelTests::cleanup()
{
}

void
Ut_AlertToneToplevelTests::alerttonetoplevelSetTitle ()
{
      AlertToneToplevel  attl(0);
      attl.setTitle( "title" );
      QCOMPARE  ( QString ("title"), attl.title() );
}

#define NUM  3
void
Ut_AlertToneToplevelTests::alerttonetoplevelSetViewMenuItems ()
{
      AlertToneToplevel  attl(0);
      QVector<MAction *>   mav;
      for (unsigned int  i =0 ; i < NUM ; i++)
      {
            mav.push_back (new MAction (0) );
      }
      attl.setViewMenuItems(mav);
      QCOMPARE ( NUM, attl.viewMenuItems().size() );
}
MApplication *app;

QTEST_APPLESS_MAIN(Ut_AlertToneToplevelTests)
