/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#include "ut_profilevalue.h"

#include <MApplication>
#include <QString>
#include <QStringList>
#include "alerttone.h"
#include "qtrackedvariant.h"
#ifdef HAVE_LIBPROFILE
#include <profiled/libprofile.h>
#endif
#include <QDebug>

#define DEBUG 
#define WARNING 
#include "../debug.h"

#ifdef HAVE_LIBPROFILE
/******************************************************************************
 * Stubbing the profile library here. We absolutely need this because we don't
 * want to be dependent on that library.
 */
char *
profile_get_type (
        const char *key)
{
    const QString  myKey = key;
    const char   *retval = NULL;

    SYS_DEBUG ("*** key = %s", SYS_STR(myKey));
    if (myKey == "clock.alarm.enabled")
        retval = "BOOLEAN";
    else if (myKey == "ringing.alert.type")
        retval = "STRING \"Ringing\" \"Silent\" \"Beep\"";
    else if (myKey == "ringing.alert.tone")
        retval = "SOUNDFILE";
    else if (myKey == "im.alert.tone")
        retval = "SOUNDFILE";
    else if (myKey == "ringing.alert.volume")
        retval = "INTEGER 0-100";
    else if (myKey == "system.sound.level")
        retval = "INTEGER 0-3";
    else if (myKey == "ringing.alert.tone")
        retval = "SOUNDFILE";
    else if (myKey == "clock.alarm.enabled")
        retval = "BOOLEAN";
    else if (myKey == "ringing.alert.type")
        retval = "STRING \"Ringing\" \"Silent\" \"Beep\"";
    else if (myKey == "ringing.alert.tone")
        retval = "SOUNDFILE";
    else if (myKey == "im.alert.tone")
        retval = "SOUNDFILE";
    else if (myKey == "ringing.alert.volume")
        retval = "INTEGER 0-100";

    if (!retval) {
        SYS_WARNING ("The stub does not know about this key. %s",
                SYS_STR(myKey));
        retval = "";
    }

    return strdup (retval);
}
#endif

/******************************************************************************
 * Ut_ProfileValue implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_profilevaluetests",
      NULL };

void
Ut_ProfileValueTests::initTestCase()
{
      m_App = new MApplication(argc, argv);
}

void
Ut_ProfileValueTests::cleanupTestCase()
{
    if (m_App)
        delete m_App;
}


void
Ut_ProfileValueTests::init()
{
      cnt = 0;
}

void
Ut_ProfileValueTests::cleanup()
{
}

void
Ut_ProfileValueTests::profilevaluePossibleValues_data()
{
      QTest::addColumn<QString>("key");
      QTest::addColumn<QVariant>("firstResult");
      QTest::addColumn<QVariant>("secondResult");
      QTest::addColumn<QVariant>("thirdResult");
      QTest::addColumn<int>("rangetype");

      QTest::newRow("clock alarm enabled") << "clock.alarm.enabled" << QVariant(0)<<QVariant(1)<<QVariant(0)<<(int)QProfileValue::List ;
      QTest::newRow("ringing.alert.type") << "ringing.alert.type" << QVariant("Ringing")<<QVariant("Silent")<<QVariant("Beep")<<(int)QProfileValue::List ;
      QTest::newRow("ringing alert tone") << "ringing.alert.tone" << QVariant(0)<<QVariant(0)<<QVariant(0)<<(int)QProfileValue::List ;
      QTest::newRow("internet message alert tone with profile") << "im.alert.tone@silent" << QVariant(0)<<QVariant(0)<<QVariant(0)<<(int)QProfileValue::List ;
      QTest::newRow("ringing alert volume") << "ringing.alert.volume" << QVariant(0)<<QVariant(100)<<QVariant(0)<<(int)QProfileValue::Interval;
      QTest::newRow("keypad sound level") << "system.sound.level" << QVariant(0)<<QVariant(3)<<QVariant(0)<<(int)QProfileValue::Interval;
}


void
Ut_ProfileValueTests::profilevaluePossibleValues()
{
      QFETCH(QString, key);
      QFETCH(QVariant, firstResult);
      QFETCH(QVariant, secondResult);
      QFETCH(QVariant, thirdResult);
      QFETCH(int, rangetype);

      QProfileValue::RangeType *p_rangeType = new QProfileValue::RangeType;
      QProfileValue  pv(key);
      QList<QVariant> result;

      result = pv.possibleValues(p_rangeType);

      if (result.size() == 2)
      {
            QCOMPARE( result.at(0).toInt (), firstResult.toInt ());
            QCOMPARE( result.at(1).toInt (), secondResult.toInt ());
      }
      else  if (result.size() == 3)
      {
            QCOMPARE( result.at(0).toString (), firstResult.toString ());
            QCOMPARE( result.at(1).toString (), secondResult.toString ());
            QCOMPARE( result.at(1).toString (), secondResult.toString());
      }

      QCOMPARE( (int)*p_rangeType, rangetype);
}

void
Ut_ProfileValueTests::profilevaluefetchFromBackend_data()
{
      QTest::addColumn<QString>("key");
      QTest::newRow("system sound level") << "system.sound.level";
      QTest::newRow("ringing.alert.tone") << "ringing.alert.tone";
}

/*
 * FIXME: fetching from the backend is not stubbed yet!
 */
void
Ut_ProfileValueTests::profilevaluefetchFromBackend()
{
#ifdef HAVE_LIBPROFILE
    QFETCH(QString, key);

    QString type (profile_get_type(key.toAscii().data()));
    QProfileValue tc(key);
    tc.fetchFromBackend ();

    SYS_DEBUG ("*** key   = %s", SYS_STR(key));
    SYS_DEBUG ("*** value = %s", SYS_STR(tc.value().toString()));
    SYS_DEBUG ("*** type  = %s", SYS_STR(type));
    if (type == "SOUNDFILE") {
        QVERIFY (!tc.value().toString().isEmpty ());
    } else if (type == "INTEGER 0-3") {
        QVERIFY (0 <= tc.value().toInt() && 3 >= tc.value().toInt());
    } else {
        SYS_WARNING ("Unhandled type: %s", SYS_STR(type));
    }
#endif
}


void
Ut_ProfileValueTests::profilevalueGetType_data()
{
      QTest::addColumn<QString>("key");
      QTest::addColumn<QString>("profile");
      QTest::addColumn<QString>("result");

      QTest::newRow("") << "clock.alarm.enabled"     << "" << "BOOLEAN";
      QTest::newRow("") << "ringing.alert.type@loud" << "loud" << "STRING";
      QTest::newRow("") << "ringing.alert.tone@beep" << "beep" << "SOUNDFILE";
      QTest::newRow("") << "im.alert.tone@silent" << "silent" << "SOUNDFILE";
      QTest::newRow("") << "ringing.alert.volume" << "" << "INTEGER";
      QTest::newRow("") << "system.sound.level"   << "" << "INTEGER";
}

void
Ut_ProfileValueTests::profilevalueGetType()
{
      QFETCH(QString, key);
      QFETCH(QString, profile);
      QFETCH(QString, result);

      QProfileValue  pv(key);

      QString outProfile;
      QStringList list;

      list = pv.getType(key, outProfile);

      QCOMPARE( list.at(0), result );
      QCOMPARE( outProfile, profile );
}

void
Ut_ProfileValueTests::profilevalueNotifyValue()
{
#ifdef HAVE_LIBPROFILE
      cnt = 0;
      QProfileValue  pv("ringing.alert.volume");
      QObject::connect(&pv, SIGNAL(changed()), this, SLOT( catchsignal() ));
      QStringList list;
      list << "general" << "silent";
      QString profile = profile_get_profile();

      QProfileValue::notifyValue( profile.toAscii(), "ringing.alert.volume", 0, 0, &pv);
      QVERIFY( cnt );

      cnt = 0;
      QProfileValue::notifyValue( profile.toAscii(), "system.sound.level", 0, 0, &pv);

      QVERIFY( !cnt );

      if (profile == list.at(0))
            profile = list.at(1);
      else
            profile = list.at(0);

      QProfileValue::notifyValue( profile.toAscii(), "system.sound.level", 0, 0, &pv);
      QVERIFY( !cnt );
#endif
}

QTEST_APPLESS_MAIN(Ut_ProfileValueTests)
