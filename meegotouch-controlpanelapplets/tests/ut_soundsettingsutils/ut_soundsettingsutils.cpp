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
#include "ut_soundsettingsutils.h"
#include "soundsettingsutils.h"


#include <QFile>
#include <QIODevice> 
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QByteArray> 
#include <QCryptographicHash>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>


int   argc = 1;
char *argv[] = {
	      (char *) "./ut_soundsettingsutils",
	            NULL };

void Ut_SoundsettingsUtils::init ()
{
}
void Ut_SoundsettingsUtils::cleanup ()
{
}
void Ut_SoundsettingsUtils::initTestCase ()
{
	m_App = new MApplication(argc, argv);
}
void Ut_SoundsettingsUtils::cleanupTestCase ()
{
	if (m_App)
         delete m_App;
}


void Ut_SoundsettingsUtils::testMountPoint()
{
    QString  mydocsdir = QDir::homePath () + "/MyDocs/";
    QCOMPARE (SoundSettings::mountPoint(), mydocsdir) ;
}

void Ut_SoundsettingsUtils::testUserSaveDir()
{
    QString ringtones_path = QDir::homePath () + "/.ringtones";
    QCOMPARE (SoundSettings::userSaveDir(), ringtones_path) ;
}


void Ut_SoundsettingsUtils::testIsTemporaryFile()
{
    QString  testfilepath = QDir::homePath () + "/MyDocs/" + "na";
    QVERIFY (SoundSettings::isTemporaryFile(testfilepath));
    QVERIFY (!(SoundSettings::isTemporaryFile("/home/felhasznalo/testfile")));
}

void Ut_SoundsettingsUtils::testIsFileCopy()
{
    QString  testfilepath = QDir::homePath () + "/.ringtones/" + "na";

    QVERIFY (SoundSettings::isFileCopy (testfilepath));
    QVERIFY (!(SoundSettings::isFileCopy("/home/felhasznalo/testfile")));
}

void Ut_SoundsettingsUtils::testSuggestedXmlFilePath()
{
    QString  testfilepath = QDir::homePath () + "/test/a"; 
    QString testxmlpath;
    SoundSettings::suggestedXmlFilePath (testfilepath, testxmlpath);

    QString  testpath = QDir::homePath () + "/test/a.xml"; 

    QCOMPARE (testxmlpath, testpath );
}

void Ut_SoundsettingsUtils::testSuggestedTargetFilePath()
{
//LATER
}

void Ut_SoundsettingsUtils::testSaveFile()
{
}


void Ut_SoundsettingsUtils::testSaveXML()
{
  t_xmlfile = "test.xml";
  t_origFileName = "orig.file";
  t_copyFileName = "copy.file";
  t_title = "test_title";

  SoundSettings::saveXML (t_xmlfile, t_origFileName, t_copyFileName, t_title);
  qDebug() << "testSaveXML" << t_xmlfile;

//Just check if we have the file, and later the testLoadXML() examine it:
  QFile testfile(t_xmlfile);
  QVERIFY (testfile.exists());

}


void Ut_SoundsettingsUtils::testLoadXML()
{

    QString m_xmlfile= t_xmlfile;
    QString m_origFileName ;
    QString m_copyFileName ;
    QString m_title ;

    SoundSettings::loadXML (t_xmlfile, m_origFileName, m_copyFileName, m_title);
    qDebug () << t_xmlfile << m_origFileName << m_copyFileName <<  m_title;
    QCOMPARE(m_origFileName, t_origFileName);
    QCOMPARE(m_copyFileName, t_copyFileName);
    QCOMPARE(m_title, t_title);


}

void Ut_SoundsettingsUtils::testRemoveUnusedFiles()
{
}


QTEST_APPLESS_MAIN(Ut_SoundsettingsUtils)

