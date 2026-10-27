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
#ifndef _ALERT_TONE_DEFAULTS_MODEL_H_
#define _ALERT_TONE_DEFAULTS_MODEL_H_

#include <QDir>
#include <QStack>
#include <QStandardItemModel>
#include <QPointer>
#include <QFileSystemWatcher>
#include <QTimer>

class AlertToneDefaultsModel : public QStandardItemModel
{
Q_OBJECT

public:
    AlertToneDefaultsModel ();
    ~AlertToneDefaultsModel ();

    typedef enum {
        NiceNameColumn  = 0,
        FullPathColumn,
        ForcedColumn
    } AlertToneColumnType;

    bool isFinished () const;

    int addSingleItem (
            const QString &niceName, 
            const QString &fileName,
            bool           forced = false);

    int findItemByFileName (const QString &FileName) const;
    int findItemByNiceName (const QString &NiceName) const;

    QString niceName (const QModelIndex &item) const;
    QString niceName (int indexRow) const;

    QString fileName (const QModelIndex &item) const;
    QString fileName (int indexRow) const;
    QString originalFileName (int indexRow) const;

    void moveItem(int from, int destination);

    void refresh (); 
    
    static QString oviRingTonesPath ();
    
    Q_SIGNALS:
        void finished ();
        void loading ();
    
    private slots:
        void fileChanged (const QString &filename);
        void directoryChanged (const QString &directory);
        void addSingleItem ();
        void dataReceived (
                const QString   &filename, 
                const QString   &title,
                const QString   &trackerId);
        void loadFromDirectory ();

    private:
        bool ensureHasDirectory (const QString &directoryPath);

    private:
    	QStack<QDir>                 m_dirStack;
	    QStack<unsigned int>         m_dirIdx;
        QPointer<QFileSystemWatcher> m_FileWatcher;
	    bool                         m_isFinished;
        QHash<QString, int>          m_FileNameCache;
        QTimer                       m_FileSystemTimer;

#ifdef UNIT_TEST
    friend class Ut_AlertToneDefaultsModelTests;
#endif
};

#endif /* !_ALERT_TONE_DEFAULTS_MODEL_H_ */
