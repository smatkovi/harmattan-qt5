/* This file is part of meegofeedbackd
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef UT_MFMANAGER_H
#define UT_MFMANAGER_H

#include <QtTest/QtTest>
#include <QObject>

class Ut_MfManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void cleanup();

    void initNoSourcesOneBackend();
    void initNoSourcesNoBackend();
    void initOneSourceOneBackend();
    void initOneSourceTwoBackends();
    void initFourBackends();
    void initTwoBackendsTwice();
    void initTwoSourcesTwice();
    void checkHardwareDetection();
    void checkDeviceStatePropagation();
};

#endif
