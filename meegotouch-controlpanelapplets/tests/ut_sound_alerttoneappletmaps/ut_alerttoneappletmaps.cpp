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
#include <QString>
#include <QStringList>
#include "ut_alerttoneappletmaps.h"
#include "alerttoneappletmaps.h"

/******************************************************************************
 * Ut_AlertToneAppletMaps implementation.
 */
void
Ut_AlertToneAppletMapsTests::init()
{
}

void
Ut_AlertToneAppletMapsTests::cleanup()
{
}

void Ut_AlertToneAppletMapsTests::alertToneAppletMapsContent_data()
{
    QTest::addColumn<QString>("key");
    QTest::addColumn<QString>("content");
    QTest::newRow("") << "ringing.alert.tone@general" << "qtn_sond_ring_tone";
    QTest::newRow("") << "email.alert.tone@general" << "qtn_sond_email_tone";
    QTest::newRow("") << "sms.alert.tone@general" << "qtn_sond_message_tone";
    QTest::newRow("") << "im.alert.tone@general" << "qtn_sond_ins_messaging";
    QTest::newRow("") << "calendar.alert.tone@general" << "qtn_sond_org_reminders";
    QTest::newRow("") << "keypad.sound.level" << "qtn_sond_keyboard";
    QTest::newRow("") << "system.sound.level" << "qtn_sond_system";
    QTest::newRow("") << "/meegotouch/input_feedback/volume/priority2/pulse" << "qtn_sond_touch_screen";
    QTest::newRow("") << "/meegotouch/input_feedback/volume/priority2/vibra" << "qtn_sond_touch_vibra";
    QTest::newRow("") << "off" << "qtn_comm_settings_off";
    QTest::newRow("") << "low" << "qtn_sond_level_1";
    QTest::newRow("") << "medium" << "qtn_sond_level_2";
    QTest::newRow("") << "high" << "qtn_sond_level_3";
}

void Ut_AlertToneAppletMapsTests::alertToneAppletMapsContent()
{
    QFETCH (QString, key);
    QFETCH (QString, content);

    QString result = AlertToneAppletMaps::map (key);
    QCOMPARE (content, result);
}

QTEST_APPLESS_MAIN (Ut_AlertToneAppletMapsTests)
