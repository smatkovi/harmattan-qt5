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

#ifndef DUMMYDRIVER_H
#define DUMMYDRIVER_H

#include <mimenginetypes.h>
#include <mimenginehwrinterface.h>
#include <QString>
#include <QStringList>
#include <QObject>

class MImEngineHwr;

/*!
  \brief DummyDriverHwr inherits from MImEngineWordsInterface(Abstract driver class) and QObject,
  it implements a dummy input method engine.

 */
class DummyDriverHwr: public QObject, public MImEngineHwrInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(DummyDriverHwr)
    Q_INTERFACES(MImEngineHwrInterface)


public:
    /*!
     * DummyDriverHwr construtor
     */
    DummyDriverHwr();

    /*!
     * DummyDriverHwr destructor
     */
    ~DummyDriverHwr();

    /*!
     *\reimp
    */   
    virtual bool init(void) ;
   
    virtual void setAreaSize(QSize size) ;
  
    virtual QSize areaSize() ;
   
    virtual void setScreenSize(QSize size) ;
 
    virtual QSize screenSize() ;
 
    virtual bool setScripts(const QStringList scripts);
    
    virtual QStringList scripts();
  
    virtual QStringList recognize();
    
    virtual void appendStroke(const QList<QPoint> stroke) ;
  
    virtual void clear();
  
    virtual QString error() ;

    virtual void train(const QString& string) ;
  
    virtual QString driver() ;

    /*!
     *\reimp_end
    */

private:
    
};
#endif //DUMMYDRIVER_H
