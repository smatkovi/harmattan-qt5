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
#include "ut_profilecontainer.h"
#include "profilecontainer.h"

#include <QString>

#define DEBUG
#define WARNING
#include "../debug.h"

/****************************************************************************
 * Unit test implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_profilecontainer",
      NULL };

void
Ut_ProfileContainer::initTestCase ()
{
    m_App = new MApplication (argc, argv);
}

void
Ut_ProfileContainer::cleanupTestCase ()
{
    m_App->deleteLater ();
    m_App = 0;
}

void
Ut_ProfileContainer::init ()
{
    m_container = new ProfileContainer (100, "title", true, 0);
}

void
Ut_ProfileContainer::cleanup ()
{
    delete m_container;
    m_container = 0;
}

void
Ut_ProfileContainer::testConstructDestruct ()
{
    // Creation test
    QVERIFY (m_container);
    QVERIFY (m_container->m_Label);
    QVERIFY (m_container->m_Button);

    cleanup ();

    m_container = new ProfileContainer (100, "title", true, 0);

    QCOMPARE (m_container->isChecked (), true);
    QCOMPARE (m_container->text (), QString ("title"));
    QCOMPARE (m_container->id (), 100);

    cleanup ();

    m_container = new ProfileContainer (200, "Loud", false, 0);

    QCOMPARE (m_container->isChecked (), false);
    QCOMPARE (m_container->text (), QString ("Loud"));
    QCOMPARE (m_container->id (), 200);
}

void
Ut_ProfileContainer::testCheckbox ()
{
    m_container->setChecked (true);
    QCOMPARE (m_container->isChecked (), true);

    m_container->setChecked (false);
    QCOMPARE (m_container->isChecked (), false);
}

void
Ut_ProfileContainer::testTexts ()
{
    m_container->setText (QString (""));
    QCOMPARE (m_container->text (), QString (""));

    m_container->setText (QString ("Demo"));
    QCOMPARE (m_container->text (), QString ("Demo"));
}

QTEST_APPLESS_MAIN (Ut_ProfileContainer);

