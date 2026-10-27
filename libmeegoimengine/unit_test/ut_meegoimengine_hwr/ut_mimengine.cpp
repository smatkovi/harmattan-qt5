/* This file is part of meegoimengine *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "ut_mimengine.h"

#include "stubdriver.h"

#include <QStringList>
#include <QtDebug>
#include <QFile>
#include <QProcess>
#include <QRegExp>


namespace
{
    const QString defaultDriverDir = "/usr/lib/meego-imengines/drivers/hwr";
}

StubDriverHwr *gInterface = NULL;
namespace
{
    const QString MyStubDriverPathKey = "MYSTUBDRIVER_PATH";
}

void Ut_MImEngineHwrInterfaceFactory::initTestCase()
{
    QStringList envs = QProcess::systemEnvironment();
    QString exp = QString("^%1=.*").arg(MyStubDriverPathKey);
    int index = envs.indexOf(QRegExp(exp, Qt::CaseInsensitive));
    QString stubDriverPath = 0;
    if (index != -1) {
        stubDriverPath = envs.at(index);
        exp = QString("^%1=").arg(MyStubDriverPathKey);
        stubDriverPath = stubDriverPath.remove(QRegExp(exp,Qt::CaseInsensitive));
        if (!stubDriverPath.isEmpty()) {
            qDebug() << "manual set driver path:" << stubDriverPath;
        }
    }

    m_subject = MImEngineFactory::instance();
    if (m_subject) {
        QStringList driverList = m_subject->drivers();
        QVERIFY(driverList.count() > 0);
        QVERIFY(driverList.indexOf("stubdriver1_hwr") != -1);
        gInterface = (StubDriverHwr *)m_subject->createEngineHwr("stubdriver1_hwr");
        QVERIFY(gInterface != NULL);        
    }
}

void Ut_MImEngineHwrInterfaceFactory::cleanupTestCase()
{
}

void Ut_MImEngineHwrInterfaceFactory::init()
{
    QStringList driverList = m_subject->drivers();
    QVERIFY(driverList.indexOf("stubdriver1_hwr") != -1);
}

void Ut_MImEngineHwrInterfaceFactory::cleanup()
{
}

void Ut_MImEngineHwrInterfaceFactory::test_setAreaSize()
{
    gInterface->stubReset();
    gInterface->setAreaSize(QSize());
    QVERIFY(gInterface->stubCallCount("setAreaSize") == 1);
}

void Ut_MImEngineHwrInterfaceFactory::test_areaSize()
{
      gInterface->stubReset();
      gInterface->stubSetReturnValue<QSize>("areaSize", QSize(100, 200));
      QVERIFY(gInterface->areaSize() == QSize(100, 200));
      QVERIFY(gInterface->stubCallCount("areaSize") == 1);
 }

  void Ut_MImEngineHwrInterfaceFactory::test_setScreenSize()
  {
      gInterface->stubReset();
      gInterface->setScreenSize(QSize());
      QVERIFY(gInterface->stubCallCount("setScreenSize") == 1);
  }


  void Ut_MImEngineHwrInterfaceFactory::test_screenSize()
  {
      gInterface->stubReset();
      gInterface->stubSetReturnValue<QSize>("screenSize", QSize(100, 200));
      QVERIFY(gInterface->screenSize() == QSize(100, 200));
      QVERIFY(gInterface->stubCallCount("screenSize") == 1);
  }

  void Ut_MImEngineHwrInterfaceFactory::test_setScripts()
  {
        gInterface->stubReset();
        gInterface->stubSetReturnValue<bool>("setScript" , false);
        QCOMPARE(gInterface->setScripts(QStringList(QString("HanA"))), false);
        QVERIFY(gInterface->stubCallCount("setScripts") == 1);

        gInterface->stubReset();
        gInterface->stubSetReturnValue<bool>("setScripts" , true);
        QCOMPARE(gInterface->setScripts(QStringList(QString("Hans"))), true);
        QVERIFY(gInterface->stubCallCount("setScripts") == 1);
  }

  void Ut_MImEngineHwrInterfaceFactory::test_scripts()
  {
      gInterface->stubReset();
      gInterface->stubSetReturnValue<QStringList>("scripts", QStringList(QString("Hans")));
      QCOMPARE(gInterface->scripts(), QStringList(QString("Hans")));
      QVERIFY(gInterface->stubCallCount("scripts") == 1);
  }

  void Ut_MImEngineHwrInterfaceFactory::test_recognize()
  {
      gInterface->stubReset();
      gInterface->stubSetReturnValue<QStringList>("recognize", QStringList(QString("三")));
      QCOMPARE(gInterface->recognize() , QStringList( QString("三")));
  	
      QVERIFY(gInterface->stubCallCount("recognize") == 1);
  }

  void Ut_MImEngineHwrInterfaceFactory::test_appendStroke()
  {       
       gInterface->stubReset();
       QList<QPoint> stroke;
       stroke<<QPoint(1,2)<<QPoint(20,80)<<QPoint(100,200);
       gInterface->appendStroke(stroke);
       QVERIFY(gInterface->stubCallCount("appendStroke") == 1);
  }

  void Ut_MImEngineHwrInterfaceFactory::test_clear()
  {
      gInterface->stubReset();
      gInterface->clear();
       QVERIFY(gInterface->stubCallCount("clear") == 1);
  }

  void Ut_MImEngineHwrInterfaceFactory::test_train()
  {
      gInterface->stubReset();
      gInterface->train(QString("测试"));
      QVERIFY(gInterface->stubCallCount("train") == 1);
  }

  void Ut_MImEngineHwrInterfaceFactory::test_drivers()
  {
      qDebug() << m_subject->drivers().count();
      QVERIFY(m_subject->drivers().count() >= 1);
  }

void Ut_MImEngineHwrInterfaceFactory::checkMultipleDriverLoad()
{
    QString driver1, driver2;

    driver1 = "stubdriver1_hwr";
    driver2 = "stubdriver2_hwr";

    QStringList driverList = m_subject->drivers();
    //check that driver exists
    QVERIFY(driverList.indexOf(driver1) != -1);
    QVERIFY(driverList.indexOf(driver2) != -1);
    QVERIFY(gInterface->driver() == driver1);
    QVERIFY(driverList.count() >= 2);

    // set second driver
    gInterface = (StubDriverHwr *)m_subject->createEngineHwr(driver2);
    QVERIFY(gInterface->driver() == driver2);

    //check that all driver exists
    QVERIFY(driverList.indexOf(driver1) != -1);
    QVERIFY(driverList.indexOf(driver2) != -1);

    gInterface = (StubDriverHwr *)m_subject->createEngineHwr(driver1);
    QVERIFY(gInterface->driver() == driver1);
    QVERIFY(driverList.indexOf(driver1) != -1);
    QVERIFY(driverList.indexOf(driver2) != -1);

    driverList = m_subject->drivers();
    QVERIFY(driverList.count() >= 2);
}
QTEST_MAIN(Ut_MImEngineHwrInterfaceFactory)
