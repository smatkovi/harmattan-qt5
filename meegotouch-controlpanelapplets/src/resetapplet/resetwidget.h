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
#ifndef RESETWIDGET_H
#define RESETWIDGET_H

#include <QPointer>
#include <MBanner>
#include "dcpwidget.h"

#include "resetbusinesslogic.h"

class MLinearLayoutPolicy;
class MHelpButton;
class MButton;

class ResetWidget : public DcpWidget
{
    Q_OBJECT

    Q_ENUMS (CurrentPlan);

public:
    ResetWidget (
            ResetBusinessLogic     *resetBusinessLogic, 
            QGraphicsWidget        *parent = 0);
    ~ResetWidget ();

    enum CurrentPlan
    {
        None = 0,
        ResetSettings,
        ClearData
    };

    void createContent ();

private slots:
    void restoreActivated ();
    void restoreConfirmed ();
    void clearActivated ();
    void clearConfirmed ();
    void showMassStorageWarning ();
    void doTheWork ();

private:
    void addButtonContainer (
            MLinearLayoutPolicy *mainLayout,
            MButton             *button1,
            MButton             *button2);
    MHelpButton *createHelpButton (const QString &link);

private:
    QPointer<ResetBusinessLogic>    m_ResetBusinessLogic;
    CurrentPlan                     m_currentPlan;
    MBanner			    *infoBanner;
    #ifdef UNIT_TEST
    friend class Ut_ResetApplet;
//    friend class Ut
    #endif
};

#endif
