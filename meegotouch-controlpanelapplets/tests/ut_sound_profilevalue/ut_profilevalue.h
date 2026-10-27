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

#ifndef UT_PROFILEVALUE_H
#define UT_PROFILEVALUE_H

#include <QtTest/QtTest>
#include <QObject>
#include <MApplication>

class Ut_ProfileValueTests: public QObject
{
Q_OBJECT

private slots:
      void init();
      void cleanup();
      void initTestCase();
      void cleanupTestCase();

      void profilevaluePossibleValues_data();
      void profilevaluePossibleValues();
      void profilevaluefetchFromBackend_data();
      void profilevaluefetchFromBackend();
      void profilevalueGetType_data();
      void profilevalueGetType();
      void profilevalueNotifyValue();
private:
      MApplication *m_App;
      int           cnt;

private slots:
      void  catchsignal(){ cnt = 1; qDebug() << "CATCH \n\n"; }
};

#endif
