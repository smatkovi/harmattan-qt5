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
#include "ut_trackedvariant.h"
#include "qtrackedvariant.h"

/******************************************************************************
 * Ut_TrackedVariant implementation.
 */
void
Ut_TrackedVariantTests::init()
{
      cnt = 0;
}
void
Ut_TrackedVariantTests::cleanup()
{
}

void
Ut_TrackedVariantTests::trackedvariantConstructor ()
{
      QTrackedVariant tv("testName");
      QCOMPARE ( tv.key() , QString ("testName") );
}

void
Ut_TrackedVariantTests::trackedvariantSetValue ()
{
      QVariant v(123);
      QTrackedVariant tv("");
      tv.set(v);
      QCOMPARE ( tv.value().toInt() , 123 );
}

void
Ut_TrackedVariantTests::trackedvariantRealSetValue()
{
      QTrackedVariant tv( "email.alert.tone" );
      tv.m_val = QVariant("test");
      tv.fetchFromBackend();
      QCOMPARE ( tv.m_val.toString() , QString("test") );
}


class testClass : public QTrackedVariant
{
public:
      testClass(QString name):QTrackedVariant(name){}
      void  emitTestSignal(){  emit_changed(); }
};


void Ut_TrackedVariantTests::trackerSignalEmited ()
{
      testClass tv("");
	QObject::connect(&tv, SIGNAL(changed()), this, SLOT(catchsignal()));
	tv.emitTestSignal();
	QVERIFY(cnt != 0);
}

MApplication *app;

QTEST_APPLESS_MAIN(Ut_TrackedVariantTests)
