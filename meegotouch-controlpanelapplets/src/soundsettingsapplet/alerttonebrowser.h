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
#ifndef _ALERT_TONE_BROWSER_H_
#define _ALERT_TONE_BROWSER_H_

#include <meegocontrolexport.h>
#include "alerttonetoplevel.h"
#include "alerttone.h"

class QString;
class MTextEdit;
class QGraphicsLinearLayout;
class MCustomContentItem;
class MAction;
class RightArrowItem;
class AlertTonePreview;
class AlertToneDefaults;
class MApplicationPage;
class MLabel;

#define USE_CONTENT_ITEM_SHEET

#ifdef USE_CONTENT_ITEM_SHEET
class ContentItemsSheet;
#endif

/*!
 * A compound widget that holds two buttons and a list widget. The two buttons
 * can be used to pick a sound file using a content picker page and load a sound
 * file from the OVI store. The list contains the default sound files, a list
 * that is created reading some specific directories of the file system.
 */
class MC_EXPORT AlertToneBrowser : public AlertToneToplevel
{
Q_OBJECT

public:
    AlertToneBrowser(AlertTone *tone, QGraphicsWidget *parent = 0);
    ~AlertToneBrowser();

    void set (
        const QVariant &filename);

    void performCancel ();
    void performAccept ();
    
signals:
    void itemSelected ();

private slots:
    void cancel();
    void accept();
    void defaultItemClicked (const QString &filename);
    void defaultsDisplayEntered();
    void setAlertTone (
            const QString &fname,
            bool           setGui = false);

    void launchMusicBrowser();
    void launchOviStore();
    void selectingMusicItem(const QString &item);
    void textChanged ();
    void browserBackButtonClicked ();
    void stopPlayingSound ();
    #ifdef USE_CONTENT_ITEM_SHEET
    void contentItemsSheetDoneClicked ();
    void deleteMusicBrowser();
    #endif

protected:
    virtual void retranslateUi();
    virtual void polishEvent ();

private:
    void startPlayingSound (const QString &filename);
    virtual void createContent();

    AlertTone                       *m_tone;
    MLabel                          *m_TitleLabel;
    RightArrowItem                  *m_my_music;
    RightArrowItem                  *m_ovi_store;

    QGraphicsLinearLayout           *m_MainLayout;
    AlertToneDefaults               *m_defaults;
    MTextEdit                       *m_LiveFilterEditor;
    QString                          currSelectedFile;
    AlertTonePreview                *m_preview;
    #ifndef USE_CONTENT_ITEM_SHEET
    MApplicationPage                *m_MusicBrowser;
    #else
    QStringList                      m_selectedItems;
    ContentItemsSheet               *m_MusicBrowser;
    #endif

    MAction                         *m_DoneAction;
    MAction                         *m_CancelAction;

#ifdef UNIT_TEST
    friend class Ut_AlertToneBrowserTests;
#endif
};

#endif /* !_ALERT_TONE_BROWSER_H_ */
