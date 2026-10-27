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
#ifndef THEMEDIALOG_H
#define THEMEDIALOG_H

#include <QObject>
#include <MDialog>
#include <QPointer>

class ThemeBusinessLogic;
class ThemeDescriptor;
class QString;
class MButton;

class ThemeDialog: public MDialog
{
Q_OBJECT

public:
    ThemeDialog (
            ThemeBusinessLogic *themeBusinessLogic,
            ThemeDescriptor    *themeDesc);
    void showDialog ();

signals:
    void themeChangeCancelled ();
    
private slots:
    void acceptClicked ();
    void cancelClicked ();
    void dialogDisappeared ();

private:
    QPointer<ThemeBusinessLogic>  m_ThemeBusinessLogic;
    ThemeDescriptor              *m_ThemeDesc;
    MButton                      *m_SelectButton;
    MButton                      *m_CancelButton;
    #ifdef UNIT_TEST
    friend class Ut_ThemeDialog;
    #endif
};

#endif
