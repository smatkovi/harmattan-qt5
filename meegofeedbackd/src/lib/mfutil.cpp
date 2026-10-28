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

#include "mfutil.h"

#include <QRegExp>

#include <QDebug>
#include <QDataStream>
#include <QDir>
#include <unistd.h>
#include <QFileInfo>
#include <QSettings>

#include <time.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

static const char gTempDirPath[] = "/tmp/mfeedbackd";

// Local private functions
QStringList getFeedbackDirsOfATheme(const QString& themeDir, const QString& appName);
QStringList getFeedbackDirsFromPath(const QString& path);

QString mfThemeDirPrefix()
{
    // the themes libmeegotouch draws with (M_THEME_DIR), else the built-in
    // directory
    QString dir = QString::fromLocal8Bit(qgetenv("M_THEME_DIR"));
    if (dir.isEmpty())
        dir = QLatin1String(THEME_DIR_PREFIX);
    while (dir.length() > 1 && dir.endsWith(QLatin1Char('/')))
        dir.chop(1);
    return dir;
}

QString mfLibDir()
{
    // the backends and sources (MEEGOFEEDBACKD_LIBDIR for a staged test)
    QString dir = QString::fromLocal8Bit(qgetenv("MEEGOFEEDBACKD_LIBDIR"));
    return dir.isEmpty() ? QLatin1String(MF_LIB_DIR) : dir;
}
QStringList getThemeInheritanceChain(const QString& themeName);
void findFeedbackFallbackPaths(const QStringList &finalFeedbacks, QVector<QStringList>& feedbackDirLists);

#ifdef MF_DEBUG
void printFallbackPaths(const QVector<QStringList>& feedbackDirLists);
#endif

static bool createTempDir()
{
    int result;
    QDir dir(gTempDirPath);

    if (dir.exists()) {
        return true;
    }

    result = mkdir(gTempDirPath,
                   S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    return result == 0;
}

QString mfTempDir()
{
    if (!createTempDir()) {
        return "";
    } else {
        return gTempDirPath;
    }
}

QStringList getFeedbackNames(const QString& appName)
{
    QStringList finalFeedbacks;
    QString baseThemeDir;

    baseThemeDir = mfThemeDirPrefix();
    baseThemeDir += QDir::separator();
    baseThemeDir += MeegoDefaultTheme;

    if (!QDir(baseThemeDir).exists()) {
        // Can't live without default dir
        qFatal("Default theme directory does not exist or contain any feedbacks.");
    }

    // Get the feedbacks of the default theme
    finalFeedbacks = getFeedbackDirsOfATheme(baseThemeDir, appName);

    // Extract the feedback names final feedback list
    for (int i = 0; i < finalFeedbacks.size(); ++i)
    {
        finalFeedbacks[i] = finalFeedbacks[i].section(QDir::separator(), -1, -1);
    }
    return finalFeedbacks;
}

QVector<QStringList> getFeedbackDirLists(const QString& theme, const QString& appName)
{
    QVector<QStringList> finalFeedbackDirLists;
    QStringList finalFeedbacks;
    QStringList themeDirs;

    themeDirs = getThemeInheritanceChain(theme);
    if (themeDirs.isEmpty()) {
        themeDirs = getThemeInheritanceChain(MeegoDefaultTheme);
        if (themeDirs.isEmpty()) {
            // Can't live without default dir
            qFatal("Default theme directory does not exist or contain any feedbacks.");
        }
    }

    // Store the default feedback list
    QString defaultThemeFeedbacksDir = themeDirs[0];
    QStringList defaultFeedbacks;

    defaultThemeFeedbacksDir += QDir::separator();
    defaultThemeFeedbacksDir += "meegotouch";
    defaultThemeFeedbacksDir += QDir::separator();
    defaultThemeFeedbacksDir += "feedbacks";
    defaultThemeFeedbacksDir += QDir::separator();
    defaultFeedbacks = getFeedbackDirsFromPath(defaultThemeFeedbacksDir);

    // Get the feedbacks of the default theme
    finalFeedbacks = getFeedbackDirsOfATheme(themeDirs[0], appName);

    // Find the original common feedback directories and remember them because
    // the getFeedbackDirsOfATheme() behaves differently for the default theme:
    // it can overwrite the common feedback directories with an application
    // specific version but we need them for the fallback mechanism.
    for (int i = 0; i < finalFeedbacks.size(); ++i)
    {
        QStringList lowestFallbackDir;

        // If a common feedback has been overridden by an application specific
        // in the base theme, add the common feedback directory to the final
        // feedbacks.
        if (i < defaultFeedbacks.size() &&
            defaultFeedbacks[i] != finalFeedbacks[i]) {
            lowestFallbackDir.append(defaultFeedbacks[i]);
        }

        lowestFallbackDir.prepend(finalFeedbacks[i]);
        finalFeedbackDirLists << lowestFallbackDir;

        // From this point on we are only interested about the names of the
        // default and final feedbacks, so extract the names here.
        if (i < defaultFeedbacks.size()) {
            defaultFeedbacks[i] = defaultFeedbacks[i].section(QDir::separator(), -1, -1);
        }
        finalFeedbacks[i] = finalFeedbacks[i].section(QDir::separator(), -1, -1);
    }

    // Go through the themes
    for (int i = 1; i < themeDirs.size(); ++i)
    {
        QStringList feedbackDirList;

        // Load the feedback directories
        feedbackDirList = getFeedbackDirsOfATheme(themeDirs[i], appName);

        // Go through the inheritance tree and add directories for feedbacks
        // with the same name.
        for (int i1 = 0; i1 < feedbackDirList.size(); ++i1) {
            for (int i2 = 0; i2 < finalFeedbacks.size(); ++i2) {
                // Only the default common feedbacks can be overridden by common feedbacks
                // in an inherited theme.
                if (!defaultFeedbacks.contains(finalFeedbacks[i2]) &&
                    feedbackDirList[i1].section(QDir::separator(), -3, -3) == "meegotouch") {
                    continue;
                }
                if (feedbackDirList[i1].section(QDir::separator(), -1) ==
                    finalFeedbacks[i2]) {
                    finalFeedbackDirLists[i2].prepend(feedbackDirList[i1]);
                    break;
                }
            }
        }
    }

    // Find possible fallbacks for feedbacks which have a dash ('-') in their name
    findFeedbackFallbackPaths(finalFeedbacks, finalFeedbackDirLists);

    #ifdef MF_DEBUG
    printFallbackPaths(finalFeedbackDirLists);
    #endif

    return finalFeedbackDirLists;
}

#ifdef MF_DEBUG
void printFallbackPaths(const QVector<QStringList>& feedbackDirLists)
{
    qDebug() << "========================\nFallback paths:";
    for (int i1 = 0; i1 < feedbackDirLists.size(); ++i1) {
        qDebug() << "Fallback path for feedback" << feedbackDirLists[i1][0].section(QDir::separator(), -1) << ":";
        for (int i2 = 0; i2 < feedbackDirLists[i1].size(); ++i2) {
            qDebug() << "   " << feedbackDirLists[i1][i2];
        }
    }
}
#endif

void findFeedbackFallbackPaths(const QStringList &finalFeedbacks, QVector<QStringList>& feedbackDirLists)
{
    // Find the fallbacks for the dash separated feedbacks
    for (int i = 0; i < finalFeedbacks.size(); ++i) {
        // If the feedback name has a dash ('-'), try to find a fallback
        // feedback for it.
        if (finalFeedbacks[i].contains(QChar('-')) == true) {
            QString baseName = finalFeedbacks[i].section(QChar('-'), 0, 0, QString::SectionSkipEmpty);
            // Fallback feedback's name does not have a dash but otherwise
            // has the same name. ("press" for "press-something"). If
            // fallback is found, append the directory list with fallback's
            // directory list.
            if (baseName.isEmpty() == false) {
                int fallbackIndex = finalFeedbacks.indexOf(baseName);
                if (fallbackIndex >= 0) {
                    feedbackDirLists[i] << feedbackDirLists[fallbackIndex];
                }
            }
        }
    }
}

QStringList getFeedbackDirsOfATheme(const QString& themeDir, const QString& appName)
{
    QString themeFeedbacksDir;
    QString appFeedbacksDir;
    QStringList feedbackDirs;
    QStringList appFeedbackDirs;

    // Construct the dir of the common feedbacks in the theme
    themeFeedbacksDir = themeDir;
    themeFeedbacksDir += QDir::separator();
    themeFeedbacksDir += "meegotouch";
    themeFeedbacksDir += QDir::separator();
    themeFeedbacksDir += "feedbacks";
    themeFeedbacksDir += QDir::separator();

    if (!QDir(themeFeedbacksDir).exists())
    {
        qDebug() << "Directory does not exist: " << themeFeedbacksDir;
    }

    // Get the common feedbacks in the theme
    feedbackDirs = getFeedbackDirsFromPath(themeFeedbacksDir);

    // Construct the dir of the application feedbacks in the theme
    themeFeedbacksDir = themeDir;
    themeFeedbacksDir += QDir::separator();
    themeFeedbacksDir += "meegotouch";
    themeFeedbacksDir += QDir::separator();
    themeFeedbacksDir += appName;
    themeFeedbacksDir += QDir::separator();
    themeFeedbacksDir += "feedbacks";
    themeFeedbacksDir += QDir::separator();

    // No application specific feedbacks
    if (!QDir(themeFeedbacksDir).exists())
    {
        // Return with the common feedback list
        return feedbackDirs;
    }

    appFeedbackDirs = getFeedbackDirsFromPath(themeFeedbacksDir);

    // Do some processing if it is the default theme
    if (themeDir.section(QDir::separator(), -1, -1) == MeegoDefaultTheme)
    {
        for (int i1 = 0; i1 < appFeedbackDirs.size(); ++i1)
        {
            bool FeedbackFound = false;

            // Find the same feedback and override it with the new location
            for (int i2 = 0; i2 < feedbackDirs.size() && !FeedbackFound; ++i2)
            {
                if (appFeedbackDirs[i1].section(QDir::separator(), -1).
                        compare(feedbackDirs[i2].section(QDir::separator(), -1)) == 0)
                {
                    feedbackDirs[i2] = appFeedbackDirs[i1];
                    FeedbackFound = true;
                }
            }

            // Add the missing feedbacks to the feedback list
            if (!FeedbackFound)
            {
                feedbackDirs += appFeedbackDirs[i1];
            }
        }
    } else {
        // Just add the application specific feedbacks to the list
        feedbackDirs += appFeedbackDirs;
    }

    return feedbackDirs;
}

QStringList getFeedbackDirsFromPath(const QString& path)
{
    // Get the list of the feedback subdirectories
    QStringList feedbackDirs = QDir(path).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    QStringList::iterator feedbacksIter;

    // Complement the locations with full path
    for (feedbacksIter = feedbackDirs.begin();
         feedbacksIter != feedbackDirs.end(); ++feedbacksIter)
    {
        // Check the allowed characters in the feedback name
        if (!(*feedbacksIter).contains(QRegExp("^[a-z0-9_.-]+$")))
        {
            qDebug() << "Ignore feedback: " << *feedbacksIter << " because it does not follow the naming conventions.";
            qDebug() << "It can contain only US-ASCII lowercase letters, numbers, underscore, dash, or period characters.";
            feedbacksIter = feedbackDirs.erase(feedbacksIter);
            feedbacksIter--;
            continue;
        }
        *feedbacksIter = QString(path).append(*feedbacksIter);
    }

    return feedbackDirs;
}

QStringList getThemeInheritanceChain(const QString& themeName)
{
    QString themeRoot;
    QString themeConfFile;
    QStringList themeDirs;
    QSettings* themeConf;

    // Construct the root dir of the theme
    themeRoot = mfThemeDirPrefix();
    themeRoot += QDir::separator();
    themeRoot += themeName;

    if (!QDir(themeRoot).exists())
    {
        qCritical() << "Theme directory does not exist: " << themeRoot;
        qCritical() << "Return with empty feedback list.";
        // Return an empty directory list if the directory does not exist
        return QStringList();
    }

    // Return the location in the case of the default theme immediately
    if (themeName.compare(MeegoDefaultTheme, Qt::CaseSensitive) == 0)
    {
        return QStringList(themeRoot);
    }

    // Construct the location of the conf file of the theme
    themeConfFile = themeRoot;
    themeConfFile += QDir::separator();
    themeConfFile += "index.theme";

    if (!QFile(themeConfFile).exists())
    {
        qDebug() << "The configuration file is missing of the theme: " << themeConfFile;
        // Return an empty directory list if the configuration file is missing
        return QStringList();
    }

    // Get the parent theme
    themeConf = new QSettings(themeConfFile, QSettings::IniFormat);

    if (!themeConf->contains("X-MeeGoTouch-Metatheme/X-Inherits") ||
        themeConf->value("X-MeeGoTouch-Metatheme/X-Inherits").toString().isEmpty())
    {
        qDebug() << "No parent theme specified for the theme: " << themeName;
        delete themeConf;
        themeConf = 0;
        // Return an empty directory list if the inheritance chain is broken
        return QStringList();
    }

    themeDirs = getThemeInheritanceChain(themeConf->value("X-MeeGoTouch-Metatheme/X-Inherits").toString());

    delete themeConf;
    themeConf = 0;

    // Return an empty directory list if the inheritance chain is broken
    if (themeDirs.isEmpty())
    {
        return QStringList();
    }
    // Add the current dir to the dir list
    themeDirs += themeRoot;
    return themeDirs;
}

static int timeStampingEnabled = -1;

bool timeStamping()
{
  if (timeStampingEnabled == 1) {
      return true;
  }
  if (timeStampingEnabled == 0) {
      return false;
  }
  FILE *fp = 0;

  fp = fopen("/home/user/.haptics_test","rb");
  if (fp) {
      timeStampingEnabled = 1;
      fclose(fp);
      return true;
  }
  timeStampingEnabled = 0;
  return false;
}

void writeTimestamp(const char* message)
{
    FILE *fr_fp = NULL;
    int fd = -1;
    struct timespec time;

    if (!message)
        return;

    clock_gettime(CLOCK_MONOTONIC, &time);
    fd = ::open("/tmp/haptics-test.txt", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    if (fd < 0 || flock(fd, LOCK_EX) < 0) {
        qDebug("writeTimestamp: Could not get lock file /tmp/haptics-test.txt");
        return;
    }

    fr_fp = fopen("/tmp/haptics-test.txt","a+");
    if (fr_fp) {
        fprintf(fr_fp, "%ld %s\n", time.tv_sec*1000+time.tv_nsec/1000000, message);
        fflush(fr_fp);
        fclose(fr_fp);
    }
    chmod("/tmp/haptics-test.txt", S_IRWXU|S_IRWXG|S_IRWXO);
    flock(fd, LOCK_UN);
    close(fd);
}

bool checkProcess(const QString& processName)
{
    QDir procDir("/proc");
    QStringList dirs = procDir.entryList(QDir::Dirs);

    // Get the directories with process PIDs
    dirs = dirs.filter(QRegExp("\\d+"));

    for (int i = 0; i < dirs.size(); ++i) {
        QFile infoFile("/proc/"+dirs[i]+"/cmdline");

        if (!infoFile.open(QIODevice::ReadOnly))
            continue;

        QDataStream processInfo(&infoFile);
        QByteArray processNameBytes(1024, 0);
        QString processNameStr;

        // Read the process name through a byte array.
        // (Treating the /proc/xx/cmdline as text file is wrong, garbage can be read
        // because of no trailing \0 after the process name.)
        processInfo.readRawData(processNameBytes.data(), processNameBytes.size());
        processNameStr = processNameBytes;

        // Check if the process is what we are looking for
        if (processName == processNameStr.section('/', -1, -1)) {
            infoFile.close();
            return true;
        }
        infoFile.close();
    }
    return false;
}
