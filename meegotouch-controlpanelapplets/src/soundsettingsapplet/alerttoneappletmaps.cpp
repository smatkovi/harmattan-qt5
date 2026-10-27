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

#include "alerttoneappletmaps.h"
#include <QMap>

#undef DEBUG
#undef WARNING
#include "../debug.h"

const char *
AlertToneAppletMaps::map (const int &i)
{
    return AlertToneAppletMaps::map (QString::number (i));
}

const char *
AlertToneAppletMaps::map (const QString &str)
{
    static QHash<QString, const char *> hashTable;

    if (hashTable.isEmpty ())
    {
        // fill the map at first time
        /* Alert tone MContentItem widget titles */
        hashTable["ringing.alert.tone@general"]         = "qtn_sond_ring_tone";
        hashTable["voip.alert.tone@general"]            = "qtn_sond_internet_tone";
        hashTable["email.alert.tone@general"]           = "qtn_sond_email_tone";
        hashTable["sms.alert.tone@general"]             = "qtn_sond_message_tone";
        hashTable["im.alert.tone@general"]              = "qtn_sond_ins_messaging";
        hashTable["calendar.alert.tone@general"]        = "qtn_sond_org_reminders";

        /* Combo box widget titles */
        hashTable["keypad.sound.level"]                 = "qtn_sond_keyboard";
        hashTable["system.sound.level"]                 = "qtn_sond_system";
        hashTable["/meegotouch/input_feedback/volume/priority2/pulse"]
                                                    ="qtn_sond_touch_screen";
        hashTable["/meegotouch/input_feedback/volume/priority2/vibra"]
                                                    = "qtn_sond_touch_vibra";

        /* GConf combo box values */
        hashTable["off"]                                = "qtn_comm_settings_off";
        hashTable["low"]                                = "qtn_sond_level_1";
        hashTable["medium"]                             = "qtn_sond_level_2";
        hashTable["high"]                               = "qtn_sond_level_3";

        /* Profile combo box values */
        hashTable["0"]                                  = "qtn_comm_settings_off";
        hashTable["1"]                                  = "qtn_sond_level_1";
        hashTable["2"]                                  = "qtn_sond_level_2";
        hashTable["3"]                                  = "qtn_sond_level_3";
    }

    return hashTable.value (str, "");
}

/*!
 * Takes a GConf stored string as an abstract representation and translates it
 * to a user interface string. Returns the UI string as a QVariant for
 * it is convenient to store in models.
 */
QVariant
AlertToneAppletMaps::mapToUiString (
    const QString &str)
{
    const QString translated = qtTrId (AlertToneAppletMaps::map(str));

    SYS_DEBUG ("%s -> %s", SYS_STR(str), SYS_STR(translated));
    return QVariant (translated);
}

/*
 * Overloaded version that accepts integer for paramater.
 */
QVariant
AlertToneAppletMaps::mapToUiString (
        int integer)
{
    const QString translated = qtTrId (AlertToneAppletMaps::map (integer));

    SYS_DEBUG ("%d -> %s", SYS_STR (integer), SYS_STR (translated));
    return QVariant (translated);
}

/* XXX: Please keep these for eng.english generation: */
#if 0
    //% "Off"
    qtTrId ("qtn_comm_settings_off");
    //% "Level 1"
    qtTrId ("qtn_sond_level_1");
    //% "Level 2"
    qtTrId ("qtn_sond_level_2");
    //% "Level 3"
    qtTrId ("qtn_sond_level_3");
    //% "System sounds"
    qtTrId ("qtn_sond_system");
    //% "Keyboard feedback"
    qtTrId ("qtn_sond_keyboard");
    //% "Touch screen tones"
    qtTrId ("qtn_sond_touch_screen");
    //% "Touch screen vibration"
    qtTrId ("qtn_sond_touch_vibra");
    //% "Ringing tone"
    qtTrId ("qtn_sond_ring_tone");
    //% "Internet tone"
    qtTrId ("qtn_sond_internet_tone");
    //% "E-mail tone"
    qtTrId ("qtn_sond_email_tone");
    //% "SMS tone"
    qtTrId ("qtn_sond_message_tone");
    //% "Instant messaging tone"
    qtTrId ("qtn_sond_ins_messaging");
    //% "Calendar reminder tone"
    qtTrId ("qtn_sond_org_reminders");
#endif

