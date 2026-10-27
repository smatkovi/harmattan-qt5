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

#include "stubdriver2.h"
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


StubDriver2Hwr::StubDriver2Hwr()
{   
}

StubDriver2Hwr::~StubDriver2Hwr()
{
}

bool StubDriver2Hwr::init()
{
    return true;
}

void StubDriver2Hwr::setAreaSize(QSize size)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QSize>(size));
    stubMethodEntered("setAreaSize", params);
}


QSize StubDriver2Hwr::areaSize()
{
    stubMethodEntered("areaSize");
    return stubReturnValue<QSize>("areaSize");
}

void StubDriver2Hwr::setScreenSize(QSize size)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QSize>(size));
    stubMethodEntered("setScreenSize", params);
}

QSize StubDriver2Hwr::screenSize()
{
    stubMethodEntered("screenSize");
    return stubReturnValue<QSize>("screenSize");
}

bool StubDriver2Hwr::setScripts(const QStringList scripts)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QStringList>(scripts));
    stubMethodEntered("setScripts", params);
    return stubReturnValue<bool>("setScripts");
}

QStringList StubDriver2Hwr::scripts()
{
    stubMethodEntered("scripts");
    return stubReturnValue<QStringList>("scripts");
}

QStringList StubDriver2Hwr::recognize()
{
    stubMethodEntered("recognize");
    return stubReturnValue<QStringList>("recognize");
}

void StubDriver2Hwr::appendStroke(const QList<QPoint> stroke)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QList<QPoint> >(stroke));
    stubMethodEntered("appendStroke", params);
}

void StubDriver2Hwr::clear()
{
    stubMethodEntered("clear");
}

void StubDriver2Hwr::train(const QString& string)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString>(string));
    stubMethodEntered("train", params);
}

QString StubDriver2Hwr::driver()
{
    stubMethodEntered("driver");
    qDebug() << "stub driver 2!";
    return "stubdriver2_hwr";
}


QString StubDriver2Hwr::error()
{
    stubMethodEntered("error");
    return QString();
}

Q_EXPORT_PLUGIN2(stubdriver2_hwr, StubDriver2Hwr);
