/***************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
**
** This file is part of meegotouch-controlpanelapplets.
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#include "brightnessbrief.h"

#include "displaybusinesslogic.h"
#include <DcpWidgetTypes>
#include <MGConfItem>

#undef DEBUG
#include "../debug.h"

static const char brightnessKey[] = "/system/osso/dsm/display/display_brightness";

BrightnessBrief::BrightnessBrief():
    m_logic (new DisplayBusinessLogic),
    m_gconfKey (0),
    m_brightness_vals (m_logic->brightnessValues ())
{
    m_gconfKey = new MGConfItem (brightnessKey);

    connect (m_gconfKey, SIGNAL (valueChanged ()),
             this, SIGNAL (valuesChanged ()));

    connect (m_logic, SIGNAL (PSMValueReceived (bool)),
             this, SLOT (PSMValueChanged (bool)));
}

BrightnessBrief::~BrightnessBrief ()
{
    delete m_gconfKey;
    m_gconfKey = 0;
    delete m_logic;
    m_logic = 0;
}

QVariant
BrightnessBrief::value() const
{
    if (m_logic->PSMValue())
    {
        return QVariant ();
    }

    return m_logic->selectedBrightnessValueIndex ();
}

QString
BrightnessBrief::valueText () const
{

    if (m_logic->PSMValue())
    {
        //% "In power save mode"
        return qtTrId ("qtn_ener_power_save_mode");
    }

    return QString ();
}

void
BrightnessBrief::setValue (const QVariant& value)
{
    m_logic->setBrightnessValue (value.toInt());
}


int
BrightnessBrief::minValue () const
{
    SYS_DEBUG ("");
    return 0;
}

int
BrightnessBrief::maxValue () const
{
    SYS_DEBUG ("");
    return m_brightness_vals.size () - 1;
}

int
BrightnessBrief::widgetTypeID () const
{

#if 0
// This is not needed anymore due to the dcps changes
    if (m_logic->PSMValue())
        return DcpWidgetType::Label;
#endif

    return DcpWidgetType::Slider;
}

void
BrightnessBrief::PSMValueChanged (
        bool enabled)
{
    Q_UNUSED (enabled);
    SYS_DEBUG ("");
    emit valuesChanged ();
}

