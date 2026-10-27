/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
**
** This file is part of duicontrolpanel.
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef BRIGHTNESSBRIEF_H
#define BRIGHTNESSBRIEF_H

#include <DcpBrief>
#include <QVariant>
#include <QList>

class MGConfItem;
class DisplayBusinessLogic;

class BrightnessBrief: public DcpBrief
{
Q_OBJECT

public:
    BrightnessBrief ();
    ~BrightnessBrief ();
    virtual int widgetTypeID () const;

    virtual int minValue () const;
    virtual int maxValue () const;
    virtual QVariant value () const;
    virtual QString valueText () const;
    virtual void setValue (const QVariant& value);

private slots:
    void PSMValueChanged (bool enabled);

private:
    DisplayBusinessLogic   *m_logic;
    MGConfItem             *m_gconfKey;
    QList<int>              m_brightness_vals;
};


#endif // BRIGHTNESSBRIEF

