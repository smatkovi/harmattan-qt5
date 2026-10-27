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

#ifndef MIMENGINEHWRINTERFACE_H
#define MIMENGINEHWRINTERFACE_H

#include "mimenginetypes.h"
#include <QString>
#include <QtPlugin>
#include <QSize>
#include <QPoint>

class QStringList;

/*!
  \class MImEngineHwrInterface

  \brief Interface of handwritting recognition features.
 */
class MImEngineHwrInterface
{
public:
    /*!
     * Constructor
     */
    MImEngineHwrInterface();

    /*!
     * Destructor
     */
    virtual ~MImEngineHwrInterface();

    /*!
     * Initialization
     *\return bool
     */
    virtual bool init(void) = 0;

    /*!
     * Sets the area size of the recognition should take place
     * \param QSize the size of the area
     */
    virtual void setAreaSize(QSize size) = 0;

    /*!
     * Gets the recognition area size
     * \return QSize
     */
    virtual QSize areaSize() = 0;

    /*!
     * Sets the screen size, this may be needed by the engine whenever
     * the device orientation changes
     * \param QSize the size of the screen
     */
    virtual void setScreenSize(QSize size) = 0;

    /*!
     * Gets the screen size
     * \return QSize
     */
    virtual QSize screenSize() = 0;

    /*!
     * Sets the scripts desired to be recognized
     * \param QStringList scripts in ISO15924 format
     * \return bool whether the operation was successful
     */
    virtual bool setScripts(const QStringList scripts) = 0;

    /*!
     * Gets the scripts requested to be recognized
     * \param QStringList
     */
    virtual QStringList scripts() = 0;

    /*!
     * Recognizes the strokes in the buffer
     * \return QString string recognized
     */
    virtual QStringList recognize() = 0;

    /*!
     * Appends a stroke into the buffer
     * \param QList<QPoint> the stroke
     */
    virtual void appendStroke(const QList<QPoint> stroke) = 0;

    /*!
     * Clears the strokes in the buffer
     */
    virtual void clear() = 0;

    /*!
     * Gets error
     *\param  N/A
     *\return QString, error
     */
    virtual QString error() = 0;

    /*!
     * Trains the stroke in the buffer as string specified in the argument
     * \param QString string
     * \return bool whether the training was successful
     */
    virtual void train(const QString &string) = 0;

    /*!
     * Gets name for current driver
     *\param  N/A
     *\return QString, current driver name
     */
    virtual QString driver() = 0;
};

Q_DECLARE_INTERFACE(MImEngineHwrInterface,
                    "com.meego.MImEngineHwrInterface/0.1")


#endif // MIMENGINEHWRINTERFACE_H

/*! \page
\section intro_sec Input Method Engine Handwriting Recognition Interface

 */
