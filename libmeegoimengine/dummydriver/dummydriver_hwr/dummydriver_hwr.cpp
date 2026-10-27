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

#include "dummydriver_hwr.h"
#include "mimenginefactory.h"
#include <QtPlugin>
#include <QLibrary>

DummyDriverHwr::DummyDriverHwr()    
{
}

DummyDriverHwr::~DummyDriverHwr()
{
}

bool DummyDriverHwr::init()
{
    return true;
}

void DummyDriverHwr::setAreaSize(QSize size) 
{
    Q_UNUSED(size);
}

QSize DummyDriverHwr::areaSize()
{
    return QSize(0, 0);
}
   
void DummyDriverHwr::setScreenSize(QSize size) 
{
    Q_UNUSED(size);
}
 
QSize DummyDriverHwr::screenSize()
{
    return QSize(0, 0);
}
 
bool DummyDriverHwr::setScripts(const QStringList scripts)
{
    Q_UNUSED(scripts);
    return true;
}
    
QStringList DummyDriverHwr::scripts()
{
    return QStringList();
}
  
QStringList DummyDriverHwr::recognize()
{
    return QStringList();
}
    
void DummyDriverHwr::appendStroke(const QList<QPoint> stroke) 
{
    Q_UNUSED(stroke);
}
  
void DummyDriverHwr::clear()
{
}

void DummyDriverHwr::train(const QString& string)
{
    Q_UNUSED(string);
} 


QString DummyDriverHwr::driver()
{
    return "dummyimdriver_hwr";
}


QString DummyDriverHwr::error()
{
    return QString();
}

Q_EXPORT_PLUGIN2(dummyimdriverhwr, DummyDriverHwr);
