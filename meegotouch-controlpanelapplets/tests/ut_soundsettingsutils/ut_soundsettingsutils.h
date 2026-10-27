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


#ifndef  UT_SOUNDSETTINGSUTILS_H
#define UT_SOUNDSETTINGSUTILS_H

#include <QtTest/QtTest>
#include <QObject> 
#include <MApplication> 

class Ut_SoundsettingsUtils : public QObject
{
Q_OBJECT

private slots:

    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testMountPoint();
    void testUserSaveDir();
    void testIsTemporaryFile();
    void testIsFileCopy();
    void testSuggestedXmlFilePath();
    void testSuggestedTargetFilePath();
    void testSaveXML();
    void testSaveFile();
    void testLoadXML();
    void testRemoveUnusedFiles();

private:
    MApplication* m_App;
    QString t_xmlfile;
    QString t_origFileName;
    QString t_copyFileName;
    QString t_title;


};

#endif
