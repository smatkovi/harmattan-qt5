/****************************************************************************
*
*  Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
*  All rights reserved.
*  Contact: Nokia Corporation (directui@nokia.com)
*  Author:  David Kedves <dkedves@blumsoft.eu>
*
*  This file is part of meegotouch-controlpanelapplets
*
*  If you have questions regarding the use of this file, please contact
*  Nokia at directui@nokia.com.
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License version 2.1 as published by the Free Software Foundation
*  and appearing in the file LICENSE.LGPL included in the packaging
*  of this file.
*
****************************************************************************/

#ifndef _MEEGOCONTROLEXPORT_H
#define _MEEGOCONTROLEXPORT_H

#undef MC_EXPORT

#if defined(BUILDING_LIBMEEGOCONTROL)
#define MC_EXPORT Q_DECL_EXPORT
#else
#define MC_EXPORT Q_DECL_IMPORT
#endif

#endif

