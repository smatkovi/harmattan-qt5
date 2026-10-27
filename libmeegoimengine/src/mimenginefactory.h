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

#ifndef MIMENGINEFACTORY_H
#define MIMENGINEFACTORY_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include "mimenginetypes.h"
#include "mimenginewordsinterface.h"
#include "mimenginehwrinterface.h"

class MImEngineFactoryPrivate;

/*!
  \class MImEngineFactory

  \brief The MImEngineFactory class provides a singleton mode interface
  to create MImEngineInterface object.
*/
class MImEngineFactory
{
    Q_DISABLE_COPY(MImEngineFactory)
    friend class Ut_MImEngineFactory;

public:
    /*!
     * gets the singleton instance for MImEngineFactory
     * note:
            the keyboards related  driver must be installed to  "/usr/lib/meego-imengines/drivers/words";
            the handwriting related driver must be installed to  "/usr/lib/meego-imengines/drivers/hwr";
     * \return the singleton instance for MImEngineFactory
     */
    static MImEngineFactory *instance();

    /*!
     * create MImEngineWordsInterface object.
     * \param driver the Engine name
     * \return MImEngineWordsInterface object pointer or null if instantiation failed
     */
    MImEngineWordsInterface *createEngineWords(const QString &driver);

    /*!
     * create MImEngineHwrInterface object.
     * \param driver the Engine name
     * \return MImEngineHwrInterface object pointer or null if instantiation failed
     */
    MImEngineHwrInterface *createEngineHwr(const QString &driver);


    /*!
     * delete MImEngineWordsInterface object.
     * \param engine MImEngineWordsInterface engine object pointer
     */
    void deleteEngine(MImEngineWordsInterface *engine);


    /*!
     * delete MImEngineHwrInterface object.
     * \param engine the MImEngineHwrInterface Engine object pointer
     */
    void deleteEngine(MImEngineHwrInterface *engine);

    /*!
     * get driver names
     * \return  driver names
     */
    QStringList drivers();

    /*!
     * MImEngineFactory destructor
     */
    ~MImEngineFactory();

private:
    MImEngineFactory();
    MImEngineFactoryPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(MImEngineFactory);
};

#endif //MIMENGINEFACTORY_H
