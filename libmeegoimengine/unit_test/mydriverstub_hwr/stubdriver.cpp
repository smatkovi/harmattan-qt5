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

#include "stubdriver.h"
#include <iostream>
#include <string>
#include <QObject>
#include <mimenginefactory.h>
#include <QStringList>
#include <QtPlugin>
#include <QtDebug>
#include <QLibrary>
#include <QSettings>
#include <QFile>
#include <QDataStream>


StubDriverHwr::StubDriverHwr()
{   
}

StubDriverHwr::~StubDriverHwr()
{
}

bool StubDriverHwr::init()
{
    return true;
}

void StubDriverHwr::setAreaSize(QSize size)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QSize>(size));
    stubMethodEntered("setAreaSize", params);
}


QSize StubDriverHwr::areaSize()
{
    stubMethodEntered("areaSize");
    return stubReturnValue<QSize>("areaSize");
}

void StubDriverHwr::setScreenSize(QSize size)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QSize>(size));
    stubMethodEntered("setScreenSize", params);
}

QSize StubDriverHwr::screenSize()
{
    stubMethodEntered("screenSize");
    return stubReturnValue<QSize>("screenSize");
}

bool StubDriverHwr::setScripts(const QStringList scripts)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QStringList>(scripts));
    stubMethodEntered("setScripts", params);
    return stubReturnValue<bool>("setScripts");
}

QStringList StubDriverHwr::scripts()
{
    stubMethodEntered("scripts");
    return stubReturnValue<QStringList>("scripts");
}

QStringList StubDriverHwr::recognize()
{
    stubMethodEntered("recognize");
    return stubReturnValue<QStringList>("recognize");
}

void StubDriverHwr::appendStroke(const QList<QPoint> stroke)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QList<QPoint> >(stroke));
    stubMethodEntered("appendStroke", params);
}

void StubDriverHwr::clear()
{
    stubMethodEntered("clear");
}

void StubDriverHwr::train(const QString& string)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString&>(string));
    stubMethodEntered("train", params);
}

QString StubDriverHwr::driver()
{
    stubMethodEntered("driver");
    qDebug() << "stub driver 1!";
    return "stubdriver1_hwr";
}


QString StubDriverHwr::error()
{
    stubMethodEntered("error");
    return QString();
}

Q_EXPORT_PLUGIN2(stubdriver1_hwr, StubDriverHwr);
