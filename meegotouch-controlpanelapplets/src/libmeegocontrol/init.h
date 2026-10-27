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

#ifndef _MEEGOCONTROL_INIT_H_
#define _MEEGOCONTROL_INIT_H_

#include <meegocontrolexport.h>
#include <gst/gst.h>

G_BEGIN_DECLS

void meegocontrol_init(int *p_argc, char **p_argv[]);
gboolean meegocontrol_init_check(int *p_argc, char **p_argv[], GError **err);

G_END_DECLS

#endif /* !_MEEGOCONTROL_INIT_H_ */
