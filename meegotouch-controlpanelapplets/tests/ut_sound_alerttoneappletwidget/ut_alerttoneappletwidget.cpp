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
#include "ut_alerttoneappletwidget.h"

#include <QString>
#include <QList>
#include <QStringList>

#include "alerttoneappletwidget.h"

#include <profiled/libprofile.h>
#include <QDebug>
#include <QGraphicsLayout>
#include "alerttonewidget.h"
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MLabel>

//#define DEBUG
#define WARNING
#include "../debug.h"

static const QString inverted = "Inverted";

/*******************************************************************************
 * Stubbing the localization.
 */
static const QString translatedSuffix ("-translated");
QString
qtTrId (
        const char  *id, 
        int          n)
{
    QString retVal (id);
    Q_UNUSED (n);

    /*
     * This will show that the string is actually trabslated.
     */
    retVal += translatedSuffix;

    SYS_DEBUG ("*** id          = %s", id);
    SYS_DEBUG ("*** translation = %s", SYS_STR(retVal));
    return retVal;
}


/******************************************************************************
 * Ut_AlertToneAppletWidget implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_alerttoneappletwidget",
      NULL };

void
Ut_AlertToneAppletWidgetTests::initTestCase()
{
      m_App = new MApplication(argc, argv);
}

void
Ut_AlertToneAppletWidgetTests::cleanupTestCase()
{
    if (m_App) {
        delete m_App;
        m_App = 0;
    }
}

void
Ut_AlertToneAppletWidgetTests::alerttoneappletwidgetConstructor ()
{
      QList<AlertTone *>  atl;
      atl.push_back (new AlertTone("calendar.alert.tone") );
      atl.push_back (new AlertTone("email.alert.tone") );
      atl.push_back (new AlertTone("im.alert.tone") );
      atl.push_back (new AlertTone("ringing.alert.tone") );
      atl.push_back (new AlertTone("sms.alert.tone") );

      AlertToneAppletWidget  ataw( atl);

      QCOMPARE( ataw.m_alertTones.size(), 5);
}


void
Ut_AlertToneAppletWidgetTests::alerttoneappletwidgetCreateContent()
{
      QList<AlertTone *>  atl;
      atl.push_back (new AlertTone("sms.alert.tone") );

      AlertToneAppletWidget  ataw( atl);
      QCOMPARE( ataw.m_alertTones.size(), 1);

      ataw.m_alertTones.push_back (new AlertTone("calendar.alert.tone") );
      ataw.m_alertTones.push_back (new AlertTone("email.alert.tone") );
      ataw.m_alertTones.push_back (new AlertTone("im.alert.tone") );
      ataw.m_alertTones.push_back (new AlertTone("ringing.alert.tone") );

      ataw.createContents();
      QCOMPARE( ataw.m_alertTones.size(), 5);
}

#if 0
void
Ut_AlertToneAppletWidgetTests::alerttoneappletwidgetCreateAlertTonesList()
{
      QList<AlertTone *>  atl;
      atl.push_back (new AlertTone("calendar.alert.tone") );
      atl.push_back (new AlertTone("email.alert.tone") );
      atl.push_back (new AlertTone("im.alert.tone") );
      atl.push_back (new AlertTone("ringing.alert.tone") );
      atl.push_back (new AlertTone("sms.alert.tone") );

      QStringList  list;
      list << "AlertTonePreview";
      list << "AlertToneWidget_calendar.alert.tone" ;
      list << "AlertToneWidget_email.alert.tone";
      list << "AlertToneWidget_im.alert.tone";
      list << "AlertToneWidget_ringing.alert.tone";
      list << "AlertToneWidget_sms.alert.tone";

      AlertToneAppletWidget  ataw( atl);

      MContainer * container = ataw.createAlertTonesList(0);
      MLayout* layout = (MLayout*)container->centralWidget()->layout ();

      MLinearLayoutPolicy*  policy=  (MLinearLayoutPolicy*)layout->landscapePolicy();

    for (int i = 0; i < policy->count();i++)
    {
        #if 0
        qDebug() << "-----------------------";
        qDebug() << ((AlertToneWidget *) policy->itemAt(i))->objectName();
        qDebug() <<  list[i];
        #endif
        QCOMPARE(((AlertToneWidget *)policy->itemAt(i))->objectName(), list[i]);
    }

}
#endif

#include  "profileintcombo.h"
void
Ut_AlertToneAppletWidgetTests::alerttoneappletwidgetCreateFeedbackList()
{
#if 0
/* XXX: TODO: Update this test to use or stub
 * the SystemInfo keyboard thingy 
 */

      QList<AlertTone *>  atl;
      atl.push_back (new AlertTone("calendar.alert.tone") );
      AlertToneAppletWidget  ataw( atl);
      MContainer * container = ataw.createFeedbackList(0);
      MLayout* layout = (MLayout*)container->centralWidget()->layout ();
      MLinearLayoutPolicy*  policy=  (MLinearLayoutPolicy*)layout->landscapePolicy();

      QStringList  list;
      list << "ProfileIntCombo_keypad.sound.level";
      list << "ProfileIntCombo_system.sound.level";
      list << "GConfStringCombo_pulse";
      list << "GConfStringCombo_vibra";

      for (int i = 0; i < policy->count();i++)
      {
            QCOMPARE( ((MComboBox *)policy->itemAt(i))->objectName(), list[i]);
      }
#endif

    QVERIFY(true);
}

QTEST_APPLESS_MAIN(Ut_AlertToneAppletWidgetTests)
