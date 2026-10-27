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
#include <QString>
#include <QStringList>
#include "ut_profileintcombo.h"
#include "profileintcombo.h"

#define DEBUG
#define WARNING
#include "../debug.h"

/*******************************************************************************
 * Stubbing the localization.
 */
static const QString translatedSuffix ("-translated");
QString
qtTrId (
        const char  *id, 
        int          n)
{
    Q_UNUSED (n);
    QString retVal (id);

    /*
     * This will show that the string is actually trabslated.
     */
    retVal += translatedSuffix;

    SYS_DEBUG ("*** id          = %s", id);
    SYS_DEBUG ("*** translation = %s", SYS_STR(retVal));
    return retVal;
}

#ifdef HAVE_LIBPROFILE
/******************************************************************************
 * Stubbing the profile library here. We absolutely need this because we don't
 * want to be dependent on that library.
 */
#include <profiled/libprofile.h>

char *
profile_get_type (
        const char *key)
{
    const QString  myKey = key;
    const char    *retval = NULL;

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
    else if (myKey == "system.sound.level")
        retval = "INTEGER 0-2";
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
    else if (myKey == "system.sound.level")
        retval = "INTEGER 0-2";
    else if (myKey == "keypad.sound.level")
        retval = "INTEGER 0-2";

    if (!retval) {
        SYS_WARNING ("The stub does not know about this key. %s",
                SYS_STR(myKey));
        retval = "";
    }

    return strdup (retval);
}

static QString  lastProfileKey;
static QVariant lastProfileValue;

int
profile_set_value_as_int (
        const char *profile,
        const char *key, 
        int val)
{
    Q_UNUSED (profile);
    lastProfileKey = key;
    lastProfileValue = val;

    SYS_DEBUG ("*** lastProfileKey = %s", SYS_STR(lastProfileKey));
    SYS_DEBUG ("*** val            = %d", val);

    return 0;
}

int
profile_get_value_as_int (
        const char *profile,
        const char *key)
{
    Q_UNUSED (key);
    Q_UNUSED (profile);
    /*
     * We only need some well known value to return so that we will not try to
     * change the combo the the index it already uses as current index.
     */
    return 0;
}
#endif

/******************************************************************************
 * Ut_ProfileintCombo implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_profileintcombo",
      NULL };

void
Ut_ProfileintComboTests::initTestCase()
{
      m_App = new MApplication(argc, argv);
}

void
Ut_ProfileintComboTests::cleanupTestCase()
{
    if (m_App)
        delete m_App;
}

void
Ut_ProfileintComboTests::profileintcomboConstructor ()
{
      ProfileIntCombo  pc("test", false);
      QCOMPARE (pc.m_val.key(), QString("test"));
}

void 
Ut_ProfileintComboTests::profileintcomboCurrentIndexChanged_data ()
{
    QTest::addColumn<QString>("profilekey");
    QTest::addColumn<int>("itemindex");
    QTest::addColumn<QString>("uistring");

    QTest::newRow("") << 
        "keypad.sound.level" <<
        1 <<
        "qtn_sond_level_1" + translatedSuffix;

    QTest::newRow("") << 
        "system.sound.level" <<
        0 <<
        "qtn_comm_settings_off" + translatedSuffix;
}

/*!
 * This test will set the various indexes 
 */
void
Ut_ProfileintComboTests::profileintcomboCurrentIndexChanged()
{
    QFETCH(QString, profilekey);
    QFETCH(int, itemindex);
    QFETCH(QString, uistring);
    
    ProfileIntCombo     pc (profilekey, false);

    /*
     * We change to the given index, then we check if the profile was actually
     * set (we are using a stub of course). In order to be sure that a change is
     * happening we do two changes, otherwise changing to the default index (0) 
     * will not initiate a signal.
     */
    SYS_DEBUG ("Changing to index %d", itemindex);
    pc.setCurrentIndex (1 - itemindex);
    pc.setCurrentIndex (itemindex);
#ifdef HAVE_LIBPROFILE
    QCOMPARE (lastProfileKey, profilekey);
    QCOMPARE (lastProfileValue.toInt(), itemindex);
#endif
    QCOMPARE (uistring, pc.currentText());
}

void 
Ut_ProfileintComboTests::profileintcomboRetranslateUi_data ()
{
    QTest::addColumn<QString>("profilekey");
    QTest::addColumn<QString>("title");
    QTest::addColumn<int>("nitems");
    QTest::addColumn<QString>("uistring1");
    QTest::addColumn<QString>("uistring2");
    QTest::addColumn<QString>("uistring3");
    QTest::addColumn<QString>("uistring4");

    QTest::newRow("") << 
        "keypad.sound.level" <<
        "qtn_sond_keyboard" + translatedSuffix <<
        3 <<
        "qtn_comm_settings_off" + translatedSuffix <<
        "qtn_sond_level_1" + translatedSuffix <<
        "qtn_sond_level_2" + translatedSuffix <<
        "";
    
    QTest::newRow("") << 
        "system.sound.level" <<
        "qtn_sond_system" + translatedSuffix <<
        4 <<
        "qtn_comm_settings_off" + translatedSuffix <<
        "qtn_sond_level_1" + translatedSuffix <<
        "qtn_sond_level_2" + translatedSuffix <<
        "qtn_sond_level_3" + translatedSuffix;

#if 0
    /*
     * This is actually not translated in AlertToneAppletMaps.
     */
    QTest::newRow("") << 
        "clock.alarm.enabled" <<
        "clock.alarm.enabled" + translatedSuffix <<
        2 <<
        "qtn_comm_settings_off" + translatedSuffix <<
        "qtn_sond_level_1" + translatedSuffix <<
        "" <<
        "";
#endif
}

/*!
 * This test will fill the combo with various data and check if the combobox
 * model holds the right Ui strings. The title of the combo is also checked to
 * be the right UI string.
 */
void
Ut_ProfileintComboTests::profileintcomboRetranslateUi()
{
    QFETCH(QString, profilekey);
    QFETCH(QString, title);
    QFETCH(int, nitems);
    QFETCH(QString, uistring1);
    QFETCH(QString, uistring2);
    QFETCH(QString, uistring3);
    QFETCH(QString, uistring4);

    ProfileIntCombo     pc (profilekey, false);
    QAbstractItemModel *model = pc.itemModel();
    QStringList         uistrings;

    uistrings << 
        uistring1 <<
        uistring2 <<
        uistring3 <<
        uistring4;
    
    SYS_DEBUG ("*** title    = %s", SYS_STR(pc.title()));
    SYS_DEBUG ("*** rowcount = %d", model->rowCount());

    QCOMPARE (title, pc.title());
    QCOMPARE (model->rowCount(), nitems);
    for (int i = 0; i < model->rowCount() && i < 4; ++i) {
        QModelIndex idx;
        QString     UiString;
        int         profileInt;

        idx = model->index (i, ProfileIntCombo::ColumnUiString);
        UiString = model->data(idx).toString();
        
        idx = model->index (i, ProfileIntCombo::ColumnProfileInt);
        profileInt = model->data(idx).toInt();

        SYS_DEBUG ("*** UiString   = %s", SYS_STR(UiString));
        SYS_DEBUG ("*** profileInt = %d", profileInt);
        QCOMPARE (uistrings[i], UiString);
    }
}

QTEST_APPLESS_MAIN(Ut_ProfileintComboTests)
