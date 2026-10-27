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

#ifndef _ALERT_TONE_DEFAULTS_H_
#define _ALERT_TONE_DEFAULTS_H_

#include <MList>
#include <MBasicListItem>
#include "alerttone.h"

class QString;
class AlertToneDefaultsCellCreator;
class AlertToneDefaultsModel;
class ShowEvent;

class MyListItem : public MBasicListItem {
Q_OBJECT
    public:
        MyListItem (
                MBasicListItem::ItemStyle style=SingleTitle, 
                QGraphicsItem *parent=NULL);
        void setup ();
};


class AlertToneDefaults : public MList
{
Q_OBJECT

Q_SIGNALS:
    void defaultItemClicked(const QString &fname);

public:
    AlertToneDefaults (AlertTone *tone, QGraphicsWidget *parent = 0);
    ~AlertToneDefaults ();
    
    void setFilterText (const QString &filter);
    bool selectAndScroll (int idx);
    bool selectAndScroll (
        const QString &fileName,
        const QString &niceName = QString(""));

public slots:
    void toneChanged();

protected:
    virtual void polishEvent ();
    virtual void showEvent (QShowEvent *event);

private slots:
    void itemClicked(const QModelIndex &item);
    void loadingFinished ();
    void userPanningStarted ();
    void checkSpinner ();

private:
    AlertTone                     *m_tone;
    AlertToneDefaultsCellCreator  *m_CellCreator;
    AlertToneDefaultsModel        *m_DefaultsModel;
    QString                        m_FileNameToSelect;
    QString                        m_NiceNameToSelect;
    bool                           m_PanningStarted;
    bool                           m_ShowingSpinner;
    bool                           m_FileFromMyMusicSelected;
#ifdef UNIT_TEST
    friend class Ut_AlertToneDefaultsTests;
#endif
};

#endif /* !_ALERT_TONE_DEFAULTS_H_ */
