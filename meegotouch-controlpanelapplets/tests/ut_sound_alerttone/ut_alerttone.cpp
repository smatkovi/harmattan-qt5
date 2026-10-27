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
#include <QString>
#include <QStringList>
#include "ut_alerttone.h"
#include "alerttone.h"
#include <QDebug>

#define DEBUG 
#define WARNING
#include "../debug.h"

static const QString validSoundFile1 = "/usr/share/sounds/ring-tones/Beep.aac";

#ifdef HAVE_LIBPROFILE
/******************************************************************************
 * libprofile stub...
 */
#include <libprofile.h>

char *stub_active_profile = NULL;
char *stub_profiles[] = { (char *) "testprofile1",
                          (char *) "testprofile2",
                          NULL };
int stub_volumelevel = 0;
int stub_vibration = 0;

extern "C" {

char **
profile_get_profiles ()
{
    SYS_WARNING ("");
    return stub_profiles;
}

void
profile_free_profiles (char **profiles)
{
    SYS_WARNING ("");
    Q_UNUSED (profiles);
}

char *
profile_get_profile ()
{
    SYS_WARNING ("");
    return stub_active_profile;
}

int
profile_set_profile (const char *profile)
{
    SYS_WARNING ("");
    if (stub_active_profile != NULL)
        delete[] stub_active_profile;

    stub_active_profile = qstrdup (profile);

    return 0;
}

int
profile_get_value_as_int (const char *profile, const char *key)
{
    SYS_WARNING ("");
    Q_UNUSED (profile);
    Q_UNUSED (key);
    return stub_volumelevel;
}

int
profile_set_value_as_int (const char *profile, const char *key, int val)
{
    SYS_WARNING ("");
    Q_UNUSED (profile);
    Q_UNUSED (key);
    stub_volumelevel = val;

    return 0;
}

int
profile_get_value_as_bool (const char *profile, const char *key)
{
    SYS_WARNING ("");
    Q_UNUSED (profile);
    Q_UNUSED (key);
    return stub_vibration;
}

int
profile_set_value_as_bool (const char *profile, const char *key, int val)
{
    SYS_WARNING ("");
    Q_UNUSED (profile);
    Q_UNUSED (key);
    stub_vibration = val;

    return 0;
}

void
profile_connection_enable_autoconnect ()
{
    SYS_WARNING ("");
}

int
profile_tracker_init ()
{
    SYS_WARNING ("");
    return 0;
}

void
profile_tracker_quit ()
{
    SYS_WARNING ("");
}

} // extern "C"

char *
profile_get_type (
        const char *key)
{
    const QString  myKey = key;
    const char   *retval = NULL;

    SYS_WARNING ("*** key = %s", SYS_STR(myKey));
    if (myKey.endsWith(".tone"))
        retval = "SOUNDFILE";
    else if (myKey == "clock.alarm.enabled")
        retval = "BOOLEAN";
    else if (myKey == "ringing.alert.type")
        retval = "STRING \"Ringing\" \"Silent\" \"Beep\"";
    else if (myKey == "ringing.alert.volume")
        retval = "INTEGER 0-100";
    else if (myKey == "system.sound.level")
        retval = "INTEGER 0-3";
    else if (myKey == "clock.alarm.enabled")
        retval = "BOOLEAN";
    else if (myKey == "ringing.alert.type")
        retval = "STRING \"Ringing\" \"Silent\" \"Beep\"";
    else if (myKey == "ringing.alert.volume")
        retval = "INTEGER 0-100";

    if (!retval) {
        SYS_WARNING ("The stub does not know about this key. %s",
                SYS_STR(myKey));
        retval = "";
    }

    return strdup (retval);
}

#endif // HAVE_LIBPROFILE

/******************************************************************************
 * Ut_AlertTone implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_alerttonetests",
      NULL };

void
Ut_AlertToneTests::initTestCase()
{
    called_alertToneChanged = false;
    m_App = new MApplication(argc, argv);

/*
 * XXX: FIXME: TODO: fix the test cases to not depend on libprofile!
 */
#ifdef HAVE_LIBPROFILE
    SYS_DEBUG ("*** Yes! we have libprofile!");
#else
    SYS_WARNING ("*** No! we don't have libprofile, some cases may fail...");
#endif
}

void
Ut_AlertToneTests::cleanupTestCase()
{
    if (m_App)
        delete m_App;
}

void
Ut_AlertToneTests::alerttonesAlertTones()
{
    QList<AlertTone *>  tones;
    tones = AlertTone::alertTones();

    for ( int i = 0 ; i <tones.size() ;++i)
    {
        SYS_DEBUG ("%s\n\n\n", SYS_STR (tones[i]->key ()));
        QVERIFY(checkIfAlarmTone (tones[i]->key()) );
    }
}



void
Ut_AlertToneTests::alerttoneFetchFromBackend()
{
      const QString testData = "xxxTESTDATAxxx";

      AlertTone at("email.alert.tone");
      at.m_niceName= testData;
      at.fetchFromBackend();
      QVERIFY (at.m_niceName != testData);
}

void
Ut_AlertToneTests::alertToneChanged()
{
    called_alertToneChanged = true;
}

/*!
 * This test will set the value an AlerTone object and check if the signal about
 * the value change is indeed emitted.
 *
 * FIXME: This test rely on the soundfile being available in the filesystem. The
 * AlertTone object will reject the value if it is not a file path of an 
 * existing file.
 */
void
Ut_AlertToneTests::alerttoneRealSetValue()
{
    AlertTone at("email.alert.tone");

    connect(&at, SIGNAL(changed()), this, SLOT(alertToneChanged()));

    at.realSetValue(QVariant(validSoundFile1));

    for (int i = 0 ; i < 300; i += 1000) {
        QTest::qWait(100);
        if (called_alertToneChanged)
            break;
    }

    called_alertToneChanged = false;
    QCOMPARE (at.value().toString (), validSoundFile1);
}

void
Ut_AlertToneTests::alerttoneMaybeUpdate()
{
      AlertTone at("email.alert.tone");
      at.m_val.clear();
      at.maybeUpdate();
      QVERIFY (at.m_val.type() != QVariant::Invalid);
}

bool
Ut_AlertToneTests::checkIfAlarmTone (QString name)
{
    return name.contains ("alert.tone");
}


QTEST_APPLESS_MAIN(Ut_AlertToneTests)

