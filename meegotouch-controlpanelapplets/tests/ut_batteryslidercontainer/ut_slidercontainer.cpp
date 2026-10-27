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
#include "ut_slidercontainer.h"
#include "slidercontainer.h"

#include <MApplication>
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include <MSlider>

#define DEBUG
#include "../debug.h"

void Ut_SliderContainer::init ()
{
    m_slider = new SliderContainer;
    // and initalize with the test values
    m_slider->initSlider (m_testValues);
}

void Ut_SliderContainer::cleanup ()
{
    delete m_slider;
    m_slider = 0;
}

static int argc = 1;
static char *app_name = (char*) "./ut_batteryslidercontainer";

void Ut_SliderContainer::initTestCase ()
{
    m_App = new MApplication (argc, &app_name);

    m_testValues <<
        "20" <<
        "40" <<
        "60" <<
        "80";
}

void
Ut_SliderContainer::cleanupTestCase ()
{
    m_App->deleteLater ();
}

void
Ut_SliderContainer::testConstructDestruct ()
{
    QVERIFY (m_slider);
}

void
Ut_SliderContainer::testUpdateSlider ()
{
    // test some valid values first:
    m_slider->updateSlider (m_testValues.last ().toInt ());
    QCOMPARE (m_slider->m_PSMSlider->value (), m_testValues.count () - 1);

    m_slider->updateSlider (m_testValues.first ().toInt ());
    QCOMPARE (m_slider->m_PSMSlider->value (), 0);

    // test an error case (invalid value), slider shouldn't change then
    m_slider->updateSlider (133);
    QCOMPARE (m_slider->m_PSMSlider->value (), 0);
}

void
Ut_SliderContainer::testSliderChanged ()
{
    QSignalSpy spy (m_slider, SIGNAL (PSMThresholdValueChanged (int)));
    QList<QVariant> args;

    // test the first item
    m_slider->sliderValueChanged (0);
    QCOMPARE (spy.count (), 1);

    args = spy.takeLast ();
    QCOMPARE (args.at (0).toString (), m_testValues.first ());

    // and the last item
    m_slider->sliderValueChanged (m_testValues.count () - 1);
    QCOMPARE (spy.count (), 1);

    args = spy.takeLast ();
    QCOMPARE (args.at (0).toString (), m_testValues.last ());
}

QTEST_APPLESS_MAIN (Ut_SliderContainer);
