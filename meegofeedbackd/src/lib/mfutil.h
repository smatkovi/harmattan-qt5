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

#ifndef MFUTIL_H
#define MFUTIL_H

#include <QString>
#include <QStringList>
#include <QVector>
#include "mfcommon.h"

/*
 * Returns path to meegofeedbackd's temporary directory.
 * If the directory couldn't be created, an empty string is returned.
 */
QString mfTempDir();

/* Returns a vector of directories for each found feedback. Feedbacks are searched
 * according to the application name and current theme. Each list of directories
 * contain a list of possible locations for the feedback in question in theme
 * inheritance order. These directories are called fallback directories.
 *
 * The list of directories is arranged so that the most inherited directory is the
 * first and the least inherited (base) theme directory is the last.
 *
 * There are two levels of fallback.
 * First, all the possible feedback locations for the feedback in question are
 * listed in descending theme inheritance order.
 * The second fallback level is determined by the name of the feedback. If there
 * is a fallback for the feedback name, the list of possible locations for
 * the fallback feedback is added behind the list of the original feedback. There
 * is a fallback feedback for a given feedback if the original feedback name
 * contains a dash ('-') somewhere in it's name and there exist a feedback with
 * the same name before the first occurrence of the dash. For example "press" is
 * a fallback feedback for a "press-something-foo" feedback.
 */
QVector<QStringList> getFeedbackDirLists(const QString& theme, const QString& appName);

/* Returns a list of string which contains the names of the feedbacks according
 * to base theme and application name. In other words returns the available feedback
 * names for the given application.
 */
QStringList getFeedbackNames(const QString& appName);

/* Checks whether the timestamping is enabled for test measurements.
 */
bool timeStamping();

/* Writes a timestamp for the test measurements.
 */
void writeTimestamp(const char* message);

/* Get the current timestamp in microseconds.
 */
inline qint64 getTimestamp()
{
    struct timespec time;

    clock_gettime(CLOCK_MONOTONIC, &time);
    return (qint64)time.tv_sec*1000000+time.tv_nsec/1000;
}

/* Check if a process is running currently.
 */
bool checkProcess(const QString& processName);

#endif
