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

#ifndef UT_ALERTTONEDEFAULTSMODEL_H
#define UT_ALERTTONEDEFAULTSMODEL_H

#include <QtTest/QtTest>
#include <QObject>
#include <MApplication>

class AlertToneDefaultsModel;

class Ut_AlertToneDefaultsModelTests : public QObject
{
Q_OBJECT
public:
      Ut_AlertToneDefaultsModelTests():filesCount1(10),filesCount2(4){m_App = 0;}
private slots:
      void init(){};
      void cleanup(){};
      void initTestCase();
      void cleanupTestCase();
      void alerttonedefaultsmodelAddSingleItem();
private:
      void  removeFiles (unsigned  int num,unsigned  int asciBase = 65, QString apendix = ".aac", QString directory = "");
      void  createFiles (unsigned  int num,unsigned  int asciBase = 65, QString apendix = ".aac", QString directory = "");
      MApplication * m_App;
      const int   filesCount1;
      const int   filesCount2;
private:
      void clearData (AlertToneDefaultsModel &atdm);
};

#endif
