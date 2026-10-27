/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
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
#include "wallpaperitrans.h"

#include <QSize>
#include <QPixmap>
#include <QPointF>

//#define DEBUG
#include "../debug.h"

/*!
 * Constructor for WallpaperITrans
 */
WallpaperITrans::WallpaperITrans () :
    m_Orientation (M::Landscape),
    m_Scale (1.0),
    m_Rotation (0.0)
{
}

/*!
 * \param orig An another WallpaperITrans instance
 * where this one should initialize its values
 *
 * An overloaded constructor for WallpaperITrans
 */
WallpaperITrans::WallpaperITrans (
        const WallpaperITrans &orig) :
        QObject ()
{
    m_Orientation    = orig.m_Orientation;
    m_Scale          = orig.m_Scale;
    m_Rotation       = orig.m_Rotation;
    m_Offset         = orig.m_Offset;
    m_ExpectedSize   = orig.m_ExpectedSize;
}

/*!
 * Copy operator for WallpaperITrans
 */
WallpaperITrans &
WallpaperITrans::operator= (
        const WallpaperITrans &rhs)
{
    if (this != &rhs) {
        m_Orientation    = rhs.m_Orientation;
        m_Scale          = rhs.m_Scale;
        m_Rotation       = rhs.m_Rotation;
        m_Offset         = rhs.m_Offset;
        m_ExpectedSize   = rhs.m_ExpectedSize;
    }

    return *this;
}

/*!
 * Scale operator for WallpaperITrans
 */
int 
WallpaperITrans::operator* (
        const int i) const
{
    //SYS_DEBUG ("m_Scale = %f", m_Scale);
    return m_Scale * i;
}

/*!
 * Offset operator for WallpaperITrans
 */
WallpaperITrans &
WallpaperITrans::operator+= (
        const QPointF &rhs)
{
    m_Offset += rhs;

    return *this;
}

/*!
 * \returns The X offset
 */
int
WallpaperITrans::x () const
{
    return (int) m_Offset.x ();
}

/*!
 * \returns The Y offset
 */
int
WallpaperITrans::y () const
{
    return (int) m_Offset.y ();
}

/*!
 * \param scale The scale value
 *
 * Method for setting the scale
 */
void 
WallpaperITrans::setScale (
        qreal scale)
{
    m_Scale = scale;
}

void
WallpaperITrans::modScale (
        int  i)
{
    m_Scale = m_Scale + (i / 1200.0);
    if (m_Scale < 0.1)
        m_Scale = 0.1;
}

/*!
 * \returns The current scale value
 */
qreal
WallpaperITrans::scale () const
{
    return m_Scale;
}

void 
WallpaperITrans::setRotation (
        qreal rotation)
{
    m_Rotation = rotation;
}

void
WallpaperITrans::modRotation (
        int  i)
{
    m_Rotation = m_Rotation + (i / 1200.0);
}

/*!
 * \returns The current scale value
 */
qreal
WallpaperITrans::rotation () const
{
    return m_Rotation;
}


/*!
 * \param size The expected size
 *
 * Method for setting the expected size
 */
void
WallpaperITrans::setExpectedSize (
        const QSize &size)
{
    SYS_DEBUG ("*** size = %dx%d", size.width(), size.height());
    m_ExpectedSize = size;
}

/*!
 * \param offset The new offset
 *
 * Method for setting the offset
 */
void
WallpaperITrans::setOffset (
        const QPointF &offset)
{
    m_Offset = offset;
}

QPointF
WallpaperITrans::offset () const
{
    return m_Offset;
}

/*!
 * \returns the expected size
 */
QSize 
WallpaperITrans::expectedSize () const
{
    return m_ExpectedSize;
}

/*!
 * \returns the expected width
 */
int
WallpaperITrans::expectedWidth () const
{
    return m_ExpectedSize.width();
}

/*!
 * \returns the expected height
 */
int
WallpaperITrans::expectedHeight () const
{
    return m_ExpectedSize.height();
}

/*!
 * Returns the orientation set for the image transformation. The orientation is
 * needed for the file saving, a separate portrait and landscape image file will
 * be saved.
 */
M::Orientation 
WallpaperITrans::orientation () const
{
    return m_Orientation;
}

/*!
 * Method for set the orientation for the image transformation
 *
 * \param orientation The wanted orientation
 */
void 
WallpaperITrans::setOrientation (
        M::Orientation orientation)
{
    m_Orientation = orientation;
}

bool
WallpaperITrans::noTransformation () const
{
    return qAbs (m_Scale - 1.0f) < 0.01f && qAbs (m_Rotation) < 0.1f &&
           qAbs (m_Offset.x ()) < 0.1f && qAbs (m_Offset.y ()) < 0.1f;
}

