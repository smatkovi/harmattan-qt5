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
#ifndef GPRSWIDGET_H
#define GPRSWIDGET_H

#include <QPointer>
#include "dcpwidget.h"

#include "gprsbusinesslogic.h"

class MLabel;
class MTextEdit;
class MButton;
class MComboBox;
class QMutex;

typedef struct t_operator_data {
    char *operator_id;
    char *country;
    char *operator_name;
    char *apn_name;
    char *username;
    char *password;
} t_operator_data;

typedef struct t_operator_data_list {
    int             count;
    t_operator_data **array;
} t_operator_data_list;

class gprsWidget : public DcpWidget
{
    Q_OBJECT

public:
    gprsWidget (
            gprsBusinessLogic     *gprsBusinessLogic,
            QGraphicsWidget        *parent = 0);
    ~gprsWidget ();

    void createContent ();
    QString labelText ();
    QString apnText ();
    QString apnUserText ();
    QString apnPasswdText ();
    QString apnActiveText ();

private slots:
    void refresh ();
    void contextReleased();
    void setApn();
    void setUser();
    void setPassword();
    void comboIndexChanged(int index_param);

protected:
    virtual void retranslateUi ();

private:
    t_operator_data_list *readOperatorConfigData();
    int getOperatorIndex ();

    QPointer<gprsBusinessLogic>    m_gprsBusinessLogic;
    MLabel                         *m_InfoLabel;
    MLabel                         *m_comboLabel;
    MLabel                         *m_MainLabel;
    MLabel                         *m_UserLabel;
    MLabel                         *m_PasswdLabel;
    MTextEdit                      *m_apnBox;
    MTextEdit                      *m_userBox;
    MTextEdit                      *m_passwdBox;
    MButton                        *m_contextButton;
    MComboBox 			   *m_combo;
    bool			    m_refresh_update;
    bool                            contextActive;
    t_operator_data_list           *m_operator_data_list;
    QMutex                         m_mutex;

    #ifdef UNIT_TEST
    friend class Ut_gprsApplet;
    #endif
};

#endif
