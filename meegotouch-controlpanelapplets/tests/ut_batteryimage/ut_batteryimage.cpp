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

#include "ut_batteryimage.h"

#include "batteryimage.h"

#include <MApplication>

#define DEBUG
#include "../debug.h"

/*
 * These are run before and after every test case
 */
void Ut_BatteryImage::init ()
{
}

void Ut_BatteryImage::cleanup ()
{
}

static int argc = 1;
static char *app_name = (char*) "./Ut_BatteryImage";

void Ut_BatteryImage::initTestCase ()
{
    SYS_DEBUG ("---->");
    m_App = new MApplication (argc, &app_name);
}

void
Ut_BatteryImage::cleanupTestCase ()
{
    m_App->deleteLater ();
}

/*!
 * It is clear now that we can not check the image sizes in here because this is
 * a unit test after all. But then what we should test? It is a constant problem
 * that the icon is not appearing, it is clear on the other hand, that it is the
 * bug in the meego theme subsystem. Should we check that the images are
 * valid... and should we create a functional test to detect if the images are
 * not loaded... or what?
 */
void 
Ut_BatteryImage::testLoadImages ()
{
    BatteryImage *image = new BatteryImage;

    image->loadImages (BatteryImage::ICON_NORMAL);
    QVERIFY (image->m_Images.size() == 9);
    
    image->loadImages (BatteryImage::ICON_POWERSAVE);
    QVERIFY (image->m_Images.size() == 9);
    
    image->loadImages (BatteryImage::ICON_CHARGING);
    QVERIFY (image->m_Images.size() == 9);

    delete image;
}

void 
Ut_BatteryImage::testAnimation ()
{
    BatteryImage *image = new BatteryImage;
    bool          indexUsed[9];
    int           level;

    /*
     * We're not animation if image is not in screen
     */
    image->setOnScreen (true);

    /*
     * Creating the image that shows the non-charging non-psm state and testing
     * if it follows the battery level.
     */
    image->stopCharging ();
    QVERIFY (image->m_ChargingSpeed == 0);
    QVERIFY (!animationIsOngoing(image));
    QVERIFY (image->m_iconCurrentSet == BatteryImage::ICON_NORMAL);

    for (int n = 8; n >= 0; --n) {
        image->updateBatteryLevel (n);
        QVERIFY (image->m_batteryLevel == n);
        QVERIFY (image->m_ImageIndex == n);
        QVERIFY (image->m_iconCurrentSet == BatteryImage::ICON_NORMAL);
    }

    /*
     * Checking the battery animation from level 0 to level 8
     */
    image->updateBatteryLevel (0);
    image->startCharging(8);
    QVERIFY (image->m_ChargingSpeed == 8);
    QVERIFY (animationIsOngoing(image));
    QVERIFY (image->m_iconCurrentSet == BatteryImage::ICON_CHARGING);

    // rsetting array
    for (int n = 0; n < 8; ++n)
        indexUsed[n] = false;
    // Gathering data
    for (int w = 0; w < 33; ++w) {
        indexUsed[image->m_ImageIndex] = true;
        QTest::qWait (9);
    }
    // Checking the values.
    for (int n = 0; n < 8; ++n) {
        QVERIFY (indexUsed[n]);
    }
    
    /*
     * Checking the cumulative charging indicator. It is a bit tricky.
     */
    for (level = 1; level < 5; ++level) {
        image->updateBatteryLevel (level);
        // Resetting the array
        for (int n = 0; n < 8; ++n)
            indexUsed[n] = false;
        // Gathering data
        for (int w = 0; w < 30; ++w) {
            indexUsed[image->m_ImageIndex] = true;
            QTest::qWait (10);
        }

        // checking the values.
        for (int n = 0; n < 8; ++n) {
            int shouldBeTrue;

            shouldBeTrue = n >= level;
            QCOMPARE (indexUsed[n], shouldBeTrue);
        }
    }

    /*
     * Checking the power save mode, with and without charging. When the
     * charging is ongoing the image will show the charging even in powersave
     * mode.
     */
    image->setPSMValue (true);
    image->updateBatteryLevel (2);
    QVERIFY (animationIsOngoing(image));
    // See NB#280113
    QVERIFY (image->m_iconCurrentSet == BatteryImage::ICON_POWERSAVE);

    image->stopCharging ();
    QVERIFY (!animationIsOngoing(image));
    QVERIFY (image->m_iconCurrentSet == BatteryImage::ICON_POWERSAVE);
    QVERIFY (image->m_ImageIndex == 2);

    delete image;
}

/******************************************************************************
 * Low level helper functions.
 */
bool 
Ut_BatteryImage::animationIsOngoing (
        BatteryImage *image)
{
    if (!image->m_timer)
        return false;

    return image->m_timer->isActive ();
}

QTEST_APPLESS_MAIN (Ut_BatteryImage);

