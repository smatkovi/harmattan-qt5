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

#include <MApplication>
#include <MApplicationWindow>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>

#include  "ut_alerttonedefaultsmodel.h"
#include  "alerttonedefaultsmodel.h"
#include "static.h"
/******************************************************************************
 * Ut_AlertToneDefaults implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_alerttonedefaultsmodel",
      NULL };

void
Ut_AlertToneDefaultsModelTests::initTestCase()
{
      /* load items */
      createFiles (filesCount1,65,".aac","loadItems");
      QDir  dir (".");
      dir.mkdir ( "complex" );
      createFiles (filesCount2,65,".aac","complex/loadItems");
      
      createFiles (filesCount2,48,".aac","numbers");
      createFiles (filesCount2,65,".zer","noAcc");
       
      m_App = new MApplication(argc, argv);
}


void
Ut_AlertToneDefaultsModelTests::cleanupTestCase()
{
      /* load items */
     removeFiles (filesCount1,65,".aac","loadItems");
     removeFiles (filesCount2,65,".aac","complex/loadItems"); 
     removeFiles (filesCount2,48,".aac","numbers");
     removeFiles (filesCount2,65,".zer","noAcc");
     
                  
      QDir  dir(".");
      dir.rmdir( "complex" );          
                      
      if (m_App)
      delete m_App;
}

void  
Ut_AlertToneDefaultsModelTests::createFiles (unsigned  int num,unsigned  int asciBase,  QString apendix, QString directory)
{
      QFile * a;
      for (unsigned  int  i =0 ;i < num; i++)
      {
            
            if (!directory.isEmpty ())
            { 
                  QDir  dir (".");
                  dir.mkdir ( directory );
                  a = new QFile(QString(".")+ QDir::separator() + directory + QDir::separator()  + QChar(i +  asciBase) + apendix );
            }
            else
            a = new QFile(QString(".")+ QDir::separator() + QChar(i +  asciBase) + apendix); 
                         
            a->open ( QIODevice::WriteOnly);
            delete a;
      }
      
}

void  
Ut_AlertToneDefaultsModelTests::removeFiles (unsigned  int num,unsigned  int asciBase,QString apendix, QString directory)
{

      for (unsigned  int  i =0 ;i < num; i++)
      {
            if (!directory.isEmpty ())
            QFile::remove (QString(".")+ QDir::separator() + directory + QDir::separator() + QChar(i +  asciBase) + apendix);
            else
            QFile::remove (QString(".")+ QDir::separator() + QChar(i +  asciBase) + apendix); 
      }      
      QDir  dir (".");
      dir.rmdir ( directory );
}

void  
Ut_AlertToneDefaultsModelTests::clearData (
        AlertToneDefaultsModel & atdm)
{
      atdm.m_dirStack.clear();
      atdm.m_dirIdx.clear();  
      atdm.m_isFinished =false;    
      atdm.removeRows (0,atdm.rowCount() );
}

void 
Ut_AlertToneDefaultsModelTests::alerttonedefaultsmodelAddSingleItem()
{
      MApplicationWindow aw;
      AlertToneDefaultsModel atdm;
      clearData (atdm);
      
      atdm.m_dirStack.push(QDir("./loadItems"));
	atdm.m_dirIdx.push(0);  
      unsigned int repeat = 3;
      for (unsigned int i=0 ;i < repeat;i++)
      atdm.addSingleItem();
      
      QVERIFY(!atdm.m_dirIdx.empty ());
      QVERIFY(atdm.m_dirIdx.top() == repeat);
      QVERIFY(atdm.rowCount ()>0);
      
      QCOMPARE(atdm.data (atdm.index(0, 0)).toString() ,(QString)"A");
      
      clearData (atdm);
      atdm.m_dirStack.push(QDir("./loadItems"));      
      atdm.m_dirIdx.push(filesCount1 + 2);

      atdm.addSingleItem();
      QVERIFY(!atdm.rowCount ());
      
      clearData (atdm);
      
      atdm.m_dirStack.push(QDir("./complex/loadItems"));
	atdm.m_dirIdx.push(0); 

      while (!atdm.m_isFinished)
      {
            atdm.addSingleItem();
      }
      QVERIFY (atdm.rowCount()==filesCount2);

      clearData (atdm);
       atdm.m_dirStack.push(QDir("./numbers"));
	atdm.m_dirIdx.push(0);      
      while (!atdm.m_isFinished)
      {
            atdm.addSingleItem();
      }
      QVERIFY (atdm.rowCount()==filesCount2);
      
      clearData (atdm);
       atdm.m_dirStack.push(QDir("./noAcc"));
	atdm.m_dirIdx.push(0);          
      while (!atdm.m_isFinished)
      {
            atdm.addSingleItem();
      }
      QVERIFY (!atdm.rowCount());
      
      clearData (atdm);
}

QTEST_APPLESS_MAIN(Ut_AlertToneDefaultsModelTests)
