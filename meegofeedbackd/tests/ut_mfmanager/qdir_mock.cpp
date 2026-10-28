/* This file is part of meegofeedbackd
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

#include <QDir>
#include <QString>

#include "globals.h"

static QString g_path;

void QDir::setPath(const QString &path)
{
    g_path = path;
}

bool QDir::exists() const
{
    return true;
}

QStringList QDir::entryList(Filters filters, SortFlags sort) const
{
    Q_UNUSED(filters);
    Q_UNUSED(sort);

    if (g_path == "/usr/lib/meegofeedbackd/threaded-backends") {
        return g_threadedBackendsDirectoryEntryList;
    } else if (g_path == "/usr/lib/meegofeedbackd/non-threaded-backends") {
        return g_nonThreadedBackendsDirectoryEntryList;
    } else if (g_path == "/usr/lib/meegofeedbackd/sources") {
        return g_sourcesDirectoryEntryList;
    } else {
        return QStringList();
    }
}

QString QDir::filePath(const QString &fileName) const
{
    return g_path + "/" + fileName;
}
