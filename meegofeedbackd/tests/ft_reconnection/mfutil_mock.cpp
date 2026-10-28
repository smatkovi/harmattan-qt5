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

#include <QStringList>
#include <QVector>

QStringList getFeedbackNames(const QString& appName)
{
    Q_UNUSED(appName)
    return QStringList("ozthewizard");
}

QVector<QStringList> getFeedbackDirLists(const QString& theme, const QString& appName)
{
    Q_UNUSED(theme)
    Q_UNUSED(appName)
    QVector<QStringList> finalFeedbackDirLists;

    // Provide existing directories in the list otherwise fails.
    finalFeedbackDirLists += QStringList("/");
    finalFeedbackDirLists += QStringList("/");

    return finalFeedbackDirLists;
}
