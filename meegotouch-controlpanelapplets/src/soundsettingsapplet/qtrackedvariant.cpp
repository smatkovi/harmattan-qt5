/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#include <QDebug>
#include "qtrackedvariant.h"

//#define DEBUG
#define WARNING 
#include "../debug.h"

/*!
 * The constructor of the class sets the key that will not be modified during
 * the lifetime of the object.
 */
QTrackedVariant::QTrackedVariant (
        const QString &key) :
    m_KeyChar (0)
{
    if (key.toUtf8().constData())
        m_KeyChar = strdup (key.toUtf8().constData());

    SYS_DEBUG ("*** key = %s", SYS_STR(key));
}

/*!
 * QTrackedVariant destructor
 */
QTrackedVariant::~QTrackedVariant()
{
    if (m_KeyChar)
        free (m_KeyChar);
}

/*!
 * Emits the changed() signal.
 */
void
QTrackedVariant::emit_changed()
{
    SYS_DEBUG ("Emitting changed()");
	emit changed();
}

/*!
 * Sets the value of the variant, calls the appropriate virtual function to
 * store the value in the profile database or in the GConf database.
 */
void
QTrackedVariant::set (
        const QVariant &newValue)
{
    realSetValue (newValue);
}

/*!
 * \returns The value of the variant.
 */
QVariant
QTrackedVariant::value()
{
    SYS_DEBUG ("*** m_val.isValid() = %s",  SYS_BOOL(m_val.isValid()));
    SYS_DEBUG ("*** m_val.isNull()  = %s",  SYS_BOOL(m_val.isNull()));
	if (!m_val.isValid() || m_val.isNull())
		fetchFromBackend();

	return m_val;
}

/*!
 * Virtual method to retrieve the data from the database backend.
 * FIXME: This method should be pure virtual.
 */
void
QTrackedVariant::fetchFromBackend()
{
    SYS_WARNING ("This virtual function does nothing!");
}

/*!
 * Virtual method to save the data to the database backend.
 * FIXME: This method should be pure virtual.
 */
void
QTrackedVariant::realSetValue (
        const QVariant &newValue)
{
	m_val = newValue;
}


/*!
 * \returns The key (id) for the variant.
 */
QString
QTrackedVariant::key() const
{
	return QString(m_KeyChar);
}

/*!
 * \returns The key (id) for the variant.
 *
 * Convenience function to retrieve the value of the variant as a null
 * terminated string.
 */
const char *
QTrackedVariant::keyChar () const
{
    return m_KeyChar;
}

