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

#ifndef STUBDRIVER2HWR_H
#define STUBDRIVER2HWR_H

#include <stubbase.h>
#include <mimenginetypes.h>
#include <QVector>
#include <QMap>
#include <QObject>
#include <mimenginehwrinterface.h>

class MImEngineHwr;
class QStringList;
class QtPlugin;
class QtDebug;
class QLibrary;
class QSettings;
class QFile;

/*!
  \class StubDriver2Hwr

  \brief StubDriver2Hwr inherit from MImEngineHwrInterface(Abstract driver class) and QObject,
  it implement the input method engine, provide the functions: e.g. hanwriting recognition.

 */
class StubDriver2Hwr: public QObject, public MImEngineHwrInterface, public StubBase
{
    Q_OBJECT
    Q_DISABLE_COPY(StubDriver2Hwr)
    Q_INTERFACES(MImEngineHwrInterface)


public:
    /*!
     * StubDriver2Hwr construtor
     *\param N/A
     *\return StubDriver2Hwr
     */
    StubDriver2Hwr();

    /*!
     * StubDriver2Hwr destructor
     *\param  N/A
     *\return N/A
     */
    ~StubDriver2Hwr();

    /*!
     *\reimp
    */     
    virtual bool init();

    virtual void setAreaSize(QSize size);

    virtual QSize areaSize();

    virtual void setScreenSize(QSize size);

    virtual QSize screenSize();

    virtual bool setScripts(const QStringList scripts);

    virtual QStringList scripts();

    virtual QStringList recognize();

    virtual void appendStroke(const QList<QPoint> stroke);

    virtual void clear();

    virtual void train(const QString& string);

    virtual QString driver();

    virtual QString error();
    /*!
     *\reimp_end
    */
};
#endif //StubDriver2Hwr_H
