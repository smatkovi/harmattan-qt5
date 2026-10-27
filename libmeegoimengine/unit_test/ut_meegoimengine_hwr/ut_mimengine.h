/* This file is part of meegoimengine *
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

#ifndef UT_MIMENGINE
#define UT_MIMENGINE

#include <QtTest/QtTest>
#include <QObject>

#include "mimenginefactory.h"
Q_DECLARE_METATYPE(MImEngineFactory *);
class Ut_MImEngineHwrInterfaceFactory : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup(); 

    void test_setScreenSize();

    void test_screenSize();

    void test_setAreaSize();

    void  test_areaSize();

    void test_setScripts();

    void  test_scripts();

    void test_recognize();

    void test_appendStroke();

    void test_clear();

    void test_train();

    void test_drivers();

    //! Test checks if multiple drivers can be loaded and changed on runtime
    void checkMultipleDriverLoad();
private:
    MImEngineFactory *m_subject;
};

#endif //MYTEST_H
