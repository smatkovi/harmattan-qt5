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
#include "ut_profilebackend.h"
#include "profilebackend.h"

#include <QByteArray>
#include <QDebug>

#define DEBUG
#include "../../src/debug.h"

/*
 * Get some consts from profilebackend.cpp
 */
extern const char *keyVibration;
extern const char *keyVolume;

#ifdef HAVE_LIBPROFILE
/******************************************************************************
 * libprofile stub...
 */
#include <libprofile.h>

char *stub_profiles[] = { (char *) "testprofile1",
                          (char *) "testprofile2",
                          (char *) "testprofile3",
                          (char *) "testprofile4",
                          NULL };
char *stub_active_profile = NULL;
int stub_volumelevel = 0;
int stub_vibration = 0;

extern "C" {

char **
profile_get_profiles ()
{
    return stub_profiles;
}

void
profile_free_profiles (char **profiles)
{
    Q_UNUSED (profiles);
}

char *
profile_get_profile ()
{
    if (! stub_active_profile)
    {
        stub_active_profile = qstrdup (stub_profiles[0]);
    }
    return stub_active_profile;
}

int
profile_set_profile (const char *profile)
{
    if (stub_active_profile != NULL)
        delete[] stub_active_profile;

    stub_active_profile = qstrdup (profile);

    return 0;
}

int
profile_get_value_as_int (const char *profile, const char *key)
{
    Q_UNUSED (profile);
    Q_UNUSED (key);
    return stub_volumelevel;
}

int
profile_set_value_as_int (const char *profile, const char *key, int val)
{
    Q_UNUSED (profile);
    Q_UNUSED (key);
    stub_volumelevel = val;

    return 0;
}

int
profile_get_value_as_bool (const char *profile, const char *key)
{
    Q_UNUSED (profile);
    Q_UNUSED (key);
    return stub_vibration;
}

int
profile_set_value_as_bool (const char *profile, const char *key, int val)
{
    Q_UNUSED (profile);
    Q_UNUSED (key);
    stub_vibration = val;

    return 0;
}

void
profile_connection_enable_autoconnect ()
{
}

int
profile_tracker_init ()
{
    return 0;
}

void
profile_tracker_quit ()
{
}

} // extern "C"
#endif // HAVE_LIBPROFILE

/******************************************************************************
 * Ut_ProfileBackend implementation.
 */
void
Ut_ProfileBackend::init()
{
#ifdef HAVE_LIBPROFILE
    // init stub:
    profile_set_profile (stub_profiles[0]);
#endif

    m_backend = ProfileBackend::getInstance ();
    m_backend->initialize ();
}

void
Ut_ProfileBackend::cleanup()
{
    delete m_backend;
    m_backend = 0;
}

void
Ut_ProfileBackend::initTestCase()
{
}

void
Ut_ProfileBackend::cleanupTestCase()
{

}

void
Ut_ProfileBackend::testActiveProfile ()
{
    QSignalSpy spy (m_backend, SIGNAL (activeProfileChanged (QString)));

    // test the set / get methods...
    m_backend->setActiveProfile (stub_profiles[0]);
    QCOMPARE (m_backend->getActiveProfile (), QString (stub_profiles[0]));

    m_backend->setActiveProfile (stub_profiles[1]);
    QCOMPARE (m_backend->getActiveProfile (), QString (stub_profiles[1]));

    // test the signalling from profiled
    m_backend->currentProfileChanged (stub_profiles[0], m_backend);
    QTest::qWait (10);
    QCOMPARE (spy.count (), 1);

    QList<QVariant> args = spy.takeFirst ();
    QCOMPARE (QString (stub_profiles[0]), args.at (0).toString ());
}

void
Ut_ProfileBackend::testGetProfiles ()
{
    QStringList testData;

    for (int i = 0; stub_profiles[i] != NULL; i++)
        testData.append (QString (stub_profiles[i]));

    QStringList backendData = m_backend->getProfiles ();

    QCOMPARE (backendData.count (), testData.count ());
}

void
Ut_ProfileBackend::testVolumeLevel ()
{
    QSignalSpy spy (m_backend, SIGNAL (volumeSettingChanged (QString, int)));
    QString testProfile = stub_profiles[1];

    // set / get methods
    m_backend->setVolumeLevel (testProfile, 20);
    QCOMPARE (m_backend->getVolumeLevel (testProfile), 20);
    m_backend->setVolumeLevel (testProfile, 100);
    QCOMPARE (m_backend->getVolumeLevel (testProfile), 100);

    // and settingchanged signal
    m_backend->changeProfileValue (stub_profiles[1], keyVolume, "33");
    QTest::qWait (10);

    QCOMPARE (spy.count (), 1);
    QList<QVariant> args = spy.takeFirst ();
    QCOMPARE (testProfile, args.at (0).toString ());
    QCOMPARE (33, args.at (1).toInt ());
}

void
Ut_ProfileBackend::testVibration ()
{
    QSignalSpy spy (m_backend, SIGNAL (vibrationSettingChanged (QString, bool)));
    QString testProfile = stub_profiles[0];

    // set / get methods
    m_backend->setVibration (testProfile, false);
    QCOMPARE (m_backend->getVibration (testProfile), false);
    m_backend->setVibration (testProfile, true);
    QCOMPARE (m_backend->getVibration (testProfile), true);

    // and settingchanged signal
    m_backend->changeProfileValue (stub_profiles[0], keyVibration, "Off");
    QTest::qWait (10);

    QCOMPARE (spy.count (), 1);
    QList<QVariant> args = spy.takeFirst ();
    QCOMPARE (testProfile, args.at (0).toString ());
    QCOMPARE (false, args.at (1).toBool ());

}

QTEST_MAIN(Ut_ProfileBackend)

