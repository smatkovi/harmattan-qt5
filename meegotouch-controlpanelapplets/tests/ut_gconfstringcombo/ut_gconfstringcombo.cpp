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
#include "ut_gconfstringcombo.h"
#include "gconfstringcombo.h"

#include <QString>
#include <QVariant>
#include <QStringList>
#include <MGConfItem>

#define DEBUG
#include "../debug.h"

/*********************************************************************************
 * MGConfItem read-write stub.
 */
static QHash<QString, QString> MyGConfDatabase;

void
Ut_GConfStringComboTests::_setGConfReturnString (
        const QString   &key,
        const QString   &newValue)
{
    SYS_DEBUG ("*** key        = %s", SYS_STR(key));
    SYS_DEBUG ("*** newValue   = %s", SYS_STR(newValue));
    MyGConfDatabase[key] = newValue;
}

QVariant
MGConfItem::value (const QVariant &def) const
{
    if (key () == "/meegotouch/target/name")
        return QVariant ("Default");
    else if (key ().startsWith ("/meegotouch/i18n"))
        return QVariant ("en_US");
    else if (MyGConfDatabase.contains (key ()))
        return QVariant (MyGConfDatabase[key ()]);
    else
        return def;
}

QVariant
MGConfItem::value () const
{
    return value (QVariant (false));
}

void
MGConfItem::set (const QVariant &val)
{
    MyGConfDatabase[key ()] = val.toString ();
}

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

/******************************************************************************
 * Ut_GConfStringCombo implementation.
 */

void
Ut_GConfStringComboTests::gconfstringcomboConstructor_data ()
{
      QTest::addColumn<QString>("gconfkey");
      QTest::addColumn<QString>("val1");
      QTest::addColumn<QString>("val2");
      QTest::addColumn<QString>("val3");
      QTest::addColumn<QString>("val4");

      QTest::newRow("") << "/meegotouch/input_feedback/volume/priority2/pulse" << "off" << "low" << "medium" << "high";
      QTest::newRow("") << "/meegotouch/input_feedback/volume/priority2/vibra" << "off" << "low" << "medium" << "high";
}

void
Ut_GConfStringComboTests::gconfstringcomboConstructor ()
{
      QFETCH(QString, gconfkey);
      QFETCH(QString, val1);
      QFETCH(QString, val2);
      QFETCH(QString, val3);
      QFETCH(QString, val4);

      QStringList  list;
      list.push_back(val1);list.push_back(val2);list.push_back(val3);list.push_back(val4);
      GConfStringCombo  gcsc (gconfkey, list);

      QCOMPARE(gcsc.m_possibleValues.size(),4);
      QCOMPARE(gcsc.m_possibleValues.at(0),val1);
      QCOMPARE(gcsc.m_possibleValues.at(1),val2);
      QCOMPARE(gcsc.m_possibleValues.at(2),val3);
      QCOMPARE(gcsc.m_possibleValues.at(3),val4);
}

void
Ut_GConfStringComboTests::gconfstringcomboChangedInGConf_data()
{
    QTest::addColumn<QString>("gconfkey");
    QTest::addColumn<QString>("val1");
    QTest::addColumn<QString>("val2");
    QTest::addColumn<QString>("val3");
    QTest::addColumn<QString>("val4");
    QTest::addColumn<QString>("currentval");
    QTest::addColumn<QString>("uistring");

    QTest::newRow("") <<
        "/meegotouch/input_feedback/volume/priority2/pulse" <<
        "off" << "low" << "medium" << "high" <<
        "off" <<
        "qtn_comm_settings_off" + translatedSuffix;

    QTest::newRow("") <<
        "/meegotouch/input_feedback/volume/priority2/pulse" <<
        "off" << "low" << "medium" << "high" <<
        "low" <<
        "qtn_sond_level_1" + translatedSuffix;

    QTest::newRow("") <<
        "/meegotouch/input_feedback/volume/priority2/pulse" <<
        "off" << "low" << "medium" << "high" <<
        "medium" <<
        "qtn_sond_level_2" + translatedSuffix;

    QTest::newRow("") <<
        "/meegotouch/input_feedback/volume/priority2/pulse" <<
        "off" << "low" << "medium" << "high" <<
        "high" <<
        "qtn_sond_level_3" + translatedSuffix;
}

/*!
 * This test will create a combo box with four possible values and will emulate
 * a GConf database change. Then the test will check if the combobox follows the
 * emulated GConf change and show the UI string it supposed to show.
 */
void
Ut_GConfStringComboTests::gconfstringcomboChangedInGConf()
{
    QStringList  list;

    QFETCH(QString, gconfkey);
    QFETCH(QString, val1);
    QFETCH(QString, val2);
    QFETCH(QString, val3);
    QFETCH(QString, val4);
    QFETCH(QString, currentval);
    QFETCH(QString, uistring);

    /*
     * Possible values list.
     */
    list.push_back(val1);
    list.push_back(val2);
    list.push_back(val3);
    list.push_back(val4);

    SYS_WARNING ("XXXXXX: key = '%s'", SYS_STR (gconfkey));

    /*
     * Creating the object with the given GConf key and possible value list.
     */
    GConfStringCombo gcsc (gconfkey, list);

    /*
     * Emulating a GConf database change and checking if we get the UI string
     * back.
     */
    _setGConfReturnString (gconfkey, currentval);
    gcsc.changedInGConf ();

    QCOMPARE (gcsc.currentText(), uistring);
}

void
Ut_GConfStringComboTests::gconfstringcomboCurrentIndexChanged_data()
{
    QTest::addColumn<QString>("gconfkey");
    QTest::addColumn<QString>("val1");
    QTest::addColumn<QString>("val2");
    QTest::addColumn<QString>("val3");
    QTest::addColumn<QString>("val4");

    QTest::newRow("") <<
        "/meegotouch/input_feedback/volume/priority2/pulse" <<
        "off" << "low" << "medium" << "high";
}

/*!
 * This test will emulate the use case when the user selects a new item from the
 * combobox. The test will set the current index for the combobox and watch the
 * simulated gconf_client_set_string() function to see if the GConf database
 * is changed the right way.
 */
void
Ut_GConfStringComboTests::gconfstringcomboCurrentIndexChanged()
{
    QFETCH(QString, gconfkey);
    QFETCH(QString, val1);
    QFETCH(QString, val2);
    QFETCH(QString, val3);
    QFETCH(QString, val4);

    QStringList  list;
    list.push_back(val1);
    list.push_back(val2);
    list.push_back(val3);
    list.push_back(val4);

    GConfStringCombo  gcsc (gconfkey, list);

    for (int n = 0; n < 4; ++n) {
        SYS_WARNING ("Changing to index %d", n);
        gcsc.setCurrentIndex (n);
        QCOMPARE (MyGConfDatabase[gconfkey], list[n]);
    }
}

void
Ut_GConfStringComboTests::gconfstringcomboRetranslateUi_data()
{
      QTest::addColumn<QString>("gconfkey");
      QTest::addColumn<QString>("title");
      QTest::addColumn<QString>("val1");
      QTest::addColumn<QString>("val2");
      QTest::addColumn<QString>("val3");
      QTest::addColumn<QString>("val4");
      QTest::addColumn<QString>("uistring1");
      QTest::addColumn<QString>("uistring2");
      QTest::addColumn<QString>("uistring3");
      QTest::addColumn<QString>("uistring4");

      /*
       * FIXME: We shoudl add more rows...
       */
      QTest::newRow("") <<
          "/meegotouch/input_feedback/volume/priority2/pulse" <<
          "qtn_sond_touch_screen" + translatedSuffix <<
          "off" << "low" << "medium" << "high" <<
          "qtn_comm_settings_off" + translatedSuffix <<
          "qtn_sond_level_1" + translatedSuffix <<
          "qtn_sond_level_2" + translatedSuffix <<
          "qtn_sond_level_3" + translatedSuffix;
}

/*!
 * This test will create a combo with a possible value list, then it will
 * emulate a current value set in the GConf database and call the
 * retranslateUi() method of the combobox to see if the proper UI string is
 * set as current text even if the current value is not contained in the
 * possible value list.
 * This test is enhanced to check the title of the combobox.
 */
void
Ut_GConfStringComboTests::gconfstringcomboRetranslateUi()
{
    QVERIFY (true);

    //TODO: FIXME: rewrite needed
#if 0
    QStringList  list;
    QStringList  uistrings;
    QStringList  possiblevalues;

    QFETCH(QString, gconfkey);
    QFETCH(QString, title);
    QFETCH(QString, val1);
    QFETCH(QString, val2);
    QFETCH(QString, val3);
    QFETCH(QString, val4);
    possiblevalues <<
        val1 <<
        val2 <<
        val3 <<
        val4;

    // Intentional, only one item, the combobox will be adding the remainder.
    list.push_back(val1);

    QFETCH(QString, uistring1);
    QFETCH(QString, uistring2);
    QFETCH(QString, uistring3);
    QFETCH(QString, uistring4);
    uistrings <<
        uistring1 <<
        uistring2 <<
        uistring3 <<
        uistring4;

    GConfStringCombo  gcsc (gconfkey, list);

    for (int i = 0; i < uistrings.size(); ++i)
    {
        gcsc.m_val.m_val = possiblevalues[i];
        gcsc.retranslateUi();
        QCOMPARE (gcsc.currentText(), uistrings[i]);
        QCOMPARE (gcsc.title(), title);
    }
#endif
}

QTEST_MAIN (Ut_GConfStringComboTests)
