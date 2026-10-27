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

#ifndef UT_ALERTTONEDEFAULTS_H
#define UT_ALERTTONEDEFAULTS_H

#include <QtTest/QtTest>
#include <QObject>
#include <MApplication>
#include  "alerttonedefaults.h"

class Ut_AlertToneDefaultsTests : public QObject
{
Q_OBJECT
public:
      Ut_AlertToneDefaultsTests(){m_App = 0;}
private slots:
      void init(){};
      void cleanup(){};
      void initTestCase();
      void cleanupTestCase();
      void alerttonedefaultsConstructor();
      void alerttonedefaultsToneChanged();
      void alerttonedefaultsItemClicked();
/*
 * FIXME: AlertToneDefaults::selectedIndex () has been removed.
 */
      //void alerttonedefaultsSelectedIndex ();
      /*
       * FIXME: AlertToneDefaults::selectedFile() has been removed.
       */
	  //void alerttonedefaultsSelectedFile ();  
/*
 * FIXME: AlertToneDefaults::rowCount() has been removed.
 */
      //void alerttonedefaultsRowCount();    
private:
      MApplication *m_App;
      AlertToneDefaults *testATD;      
private slots:
	  void receiveItemClicked(const QString & itemName);
};

#endif
