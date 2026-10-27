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
#ifndef UT_ALERTTONEPREVIEW_H
#define UT_ALERTTONEPREVIEW_H

#include <QtTest/QtTest>
#include <QObject>
#include <MApplication>

class Ut_AlertTonePreviewTests : public QObject
{
Q_OBJECT
private slots:
      void init(){};
      void cleanup(){};
      void initTestCase();
      void cleanupTestCase();
      void alerttonepreviewConstructor ();
      void alerttonepreviewfname();
      void alerttonepreviewProfileToGstVolume();
      void alerttonepreviewProfileVolumeChanged();
private:
      MApplication * m_App;
      int m_argc;
      char **m_argv;
private slots:
};

#endif
