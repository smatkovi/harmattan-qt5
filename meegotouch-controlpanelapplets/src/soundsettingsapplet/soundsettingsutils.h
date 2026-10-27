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
#ifndef SOUNDSETTINGSUTILS_H
#define SOUNDSETTINGSUTILS_H

#include <QString>
#include <QSet>

#define COPY_SOUND_FILES

namespace SoundSettings 
{
    const QString OptDir ("/opt");
    
    QString mountPoint ();
    QString userSaveDir ();

    bool isTemporaryFile (const QString &filePath);
    bool isFileCopy (const QString &filePath);

    void suggestedXmlFilePath (
            const QString &filePath,
            QString       &xmlFilePath);

    void suggestedTargetFilePath (
            const QString &filePath,
            QString       &baseDir,
            QString       &fileName,
            QString       &xmlFileName);

    QStringList customAlertToneDirs ();

    QString saveFile (
            const QString &filePath);

    bool loadXML (
            const QString   &xmlFileName,
            QString         &origFileName,
            QString         &copyFileName,
            QString         &title);

    void saveXML (
            const QString   &xmlFileName,
            const QString   &origFileName,
            const QString   &copyFileName,
            const QString   &title);

    void removeUnusedFiles (
        QSet<QString>   &usedFiles);

    void customAlarmSoundFiles (QSet<QString> &files);

};
#endif
