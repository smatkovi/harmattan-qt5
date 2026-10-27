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

#include "init.h"

void
meegocontrol_init (int *p_argc, char **p_argv[])
{
    gst_init(p_argc, p_argv);
}

gboolean
meegocontrol_init_check (int *p_argc, char **p_argv[], GError **err)
{
    return gst_init_check (p_argc, p_argv, err);
}

