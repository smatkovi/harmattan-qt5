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

#include "gprswidget.h"

#include <QGraphicsLinearLayout>

#include <MImageWidget>
#include <MLabel>
#include <MSeparator>
#include <QGraphicsLinearLayout>
#include <MStylableWidget>

#include <MButton>
#include <MTextEdit>
#include <MComboBox>
#include <QMutex>

#undef DEBUG
#define WARNING
#include "../debug.h"

gprsWidget::gprsWidget (
        gprsBusinessLogic     *gprsBusinessLogic,
        QGraphicsWidget        *parent) :
    DcpWidget (parent),
    m_gprsBusinessLogic (gprsBusinessLogic)
{
    m_refresh_update = true;
    m_operator_data_list  = readOperatorConfigData();
    createContent ();

    connect (m_gprsBusinessLogic, SIGNAL (refreshNeeded ()), SLOT (refresh ()));

    retranslateUi ();
}

gprsWidget::~gprsWidget ()
{
    int              ii;
    t_operator_data  *data;

    if (m_operator_data_list != NULL) {
        for (ii = 0; ii < m_operator_data_list->count; ii++) {
            data   = m_operator_data_list->array[ii];
            if (data != NULL) {
                free(data->country);
                free(data->operator_id);
                free(data->operator_name);
                free(data->apn_name);
                free(data->username);
                free(data->password);
                free(data);
            }
        }
        free(m_operator_data_list->array);
        free(m_operator_data_list);
        m_operator_data_list = NULL;
    }
}

QString get_formatted_comborow_text(t_operator_data *data) {
    QString country(data->country);
    QString op(data->operator_name);
    QString apn(data->apn_name);
    QString ret_val;

    if (country.length() > 0) {
        ret_val.append(country);
    }
    if (op.length() > 0) {
        if (ret_val.length() > 0) {
            ret_val.append(" - ");
        }
        ret_val.append(op);
    }
    if (apn.length() > 0) {
        if (ret_val.length() > 0) {
            ret_val.append(" - ");
        }
        ret_val.append(apn);
    }
    return ret_val;
}

t_operator_data_list *gprsWidget::readOperatorConfigData()
{
    QString                 configFilePath = "/etc/meegotouch/connections/gprs/operators";
    QString                 line;
    int                     ii;
    t_operator_data_list    *ret_val;
    t_operator_data         *data;
    QString                 txt;

    data                = (t_operator_data *)malloc(sizeof(t_operator_data));
    data->country       = strdup("");
    data->operator_id   = strdup("0");
    data->operator_name = strdup("Custom");
    data->apn_name      = strdup("internet");
    data->username      = NULL;
    data->password      = NULL;
    ret_val 		= (t_operator_data_list *)calloc(1, sizeof(t_operator_data_list));
    ret_val->count  	= 1;
    ret_val->array  	= (t_operator_data **)malloc(ret_val->count * sizeof(t_operator_data *));
    ret_val->array[0]	= data;

    QFile file(configFilePath); // file handle
    if (file.exists() == true) {        
        if (file.open(QIODevice::ReadOnly) == true) {
            QTextStream textStream(&file);
            while(textStream.atEnd() == false) {
                line    = textStream.readLine();
                if (line.startsWith('#') == false) {
                    QStringList str_list = line.split("|");
                    if (str_list.size() > 1) {
                        data                = (t_operator_data *)malloc(sizeof(t_operator_data));
                        data->country       = NULL;
                        data->operator_id   = NULL;
                        data->operator_name = NULL;
                        data->apn_name      = NULL;
                        data->username      = NULL;
                        data->password      = NULL;

                        for (ii = 0; ii < str_list.size(); ii++) {
                            switch(ii) {
                                case 0:
                                    data->operator_id  = strdup(str_list.at(ii).toAscii().data());
                                    break;
                                case 1:
                                    data->country = strdup(str_list.at(ii).toAscii().data());
                                    break;
                                case 2:
                                    data->operator_name   = strdup(str_list.at(ii).toAscii().data());
                                    break;
                                case 3:
                                    data->apn_name = strdup(str_list.at(ii).toAscii().data());
                                    break;
                                case 4:
                                    data->username  = strdup(str_list.at(ii).toAscii().data());
                                    break;
                                case 5:
                                    data->password = strdup(str_list.at(ii).toAscii().data());
                                    break;
                                default:
                                    // no more data expected
                                    break;
                            }
                        }
                        ret_val->array  = (t_operator_data **)realloc(ret_val->array, (ret_val->count + 1) * sizeof(t_operator_data *));
                        ret_val->array[ret_val->count] = data;
                        ret_val->count++;
                    }
                }
            }
        }
        else {
            //printf("Could not open operators configuration file: %s\n", configFilePath.toAscii().data());
        }
    }
    else {
        //printf("Could not get handle to operators configuration file: %s\n", configFilePath.toAscii().data());
    }
    return ret_val;
}

int
gprsWidget::getOperatorIndex () {
    int ret_val;
    int ii;
    QString opName1;
    QString apnName1;
    t_operator_data *data;

    ret_val = 0;
    opName1 = m_gprsBusinessLogic->OperatorName ();
    apnName1 = apnText();
    if (m_operator_data_list != NULL) {
        for (ii = 0; ii < m_operator_data_list->count; ii++) {
            data   = m_operator_data_list->array[ii];
	    QString opName2(data->operator_name);
	    QString apnName2(data->apn_name);
            if (opName1.compare(opName2, Qt::CaseInsensitive) == 0) {
                if (ret_val == 0)
                    ret_val = ii; // select at least a combobox row where the operator name matches
                if (apnName1.compare(apnName2, Qt::CaseInsensitive) == 0) {
                    // exact match, stop searching
                    ret_val = ii;
                    break;
		}
	    }
        }
    }
    return ret_val;
}

void
gprsWidget::createContent ()
{

    //TODO: Check if modem is powered. If not add note.

    QGraphicsLinearLayout   *layout;
    int                     ii;
    t_operator_data         *data;

    layout = new QGraphicsLinearLayout (Qt::Vertical);
    layout->setContentsMargins (0., 0., 0., 0.);


    m_InfoLabel = new MLabel;
    m_InfoLabel->setColor(QColor(222,222,222,255));
    //layout->addItem (m_InfoLabel);
/*
    m_comboLabel = new MLabel;
    m_comboLabel->setWordWrap (true);
    m_comboLabel->setColor(QColor(222,222,222,255));
    m_comboLabel->setText(tr("Connection selection"));
    layout->addItem (m_comboLabel);
*/
    m_combo = new MComboBox();
    m_combo->setStyleName("CommonComboBoxInverted");
    m_combo->setTitle("Tap to select connection");
    if (m_operator_data_list != NULL) {
        for (ii = 0; ii < m_operator_data_list->count; ii++) {
            data   = m_operator_data_list->array[ii];
            if (data != NULL) {
                QString txt = get_formatted_comborow_text(data);
                m_combo->addItem(txt);
            }
        }
    }
    m_combo->setCurrentIndex(0);
    connect (m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
    layout->addItem(m_combo);
    layout->addItem (new MSeparator (this));

    m_MainLabel = new MLabel;
    m_MainLabel->setWordWrap (true);
    m_MainLabel->setColor(QColor(222,222,222,255));
    m_MainLabel->setText(tr("Access point name"));
    layout->addItem (m_MainLabel);

    //layout->addStretch ();
    m_apnBox = new MTextEdit(MTextEditModel::SingleLine, QString(), this);
    connect (m_apnBox, SIGNAL (textChanged ()), SLOT (setApn ()));
    layout->addItem(m_apnBox);

    QGraphicsLinearLayout   *layoutUserName;
    layoutUserName = new QGraphicsLinearLayout (Qt::Vertical);
    layoutUserName->setContentsMargins (0., 0., 0., 0.);
    layoutUserName->setSpacing(0);

    m_UserLabel = new MLabel;
    m_UserLabel->setWordWrap (true);
    layoutUserName->addItem (m_UserLabel);
    m_UserLabel->setColor(QColor(222,222,222,255));
    m_UserLabel->setText(tr("User name"));

    m_userBox = new MTextEdit(MTextEditModel::SingleLine, QString(), this);
    connect (m_userBox, SIGNAL (textChanged ()), SLOT (setUser ()));
    layoutUserName->addItem(m_userBox);
    layoutUserName->setAlignment(m_userBox, Qt::AlignTop);

    QGraphicsLinearLayout   *layoutPw;
    layoutPw = new QGraphicsLinearLayout (Qt::Vertical);
    layoutPw->setContentsMargins (0., 0., 0., 0.);
    layoutPw->setSpacing(0);

    m_PasswdLabel = new MLabel;
    m_PasswdLabel->setWordWrap (true);
    layoutPw->addItem (m_PasswdLabel);
    m_PasswdLabel->setColor(QColor(222,222,222,255));
    m_PasswdLabel->setText(tr("Password"));

    m_passwdBox = new MTextEdit(MTextEditModel::SingleLine, QString(), this);
    connect (m_passwdBox, SIGNAL (textChanged ()), SLOT (setPassword ()));
    layoutPw->addItem(m_passwdBox);

    QGraphicsLinearLayout   *layoutUserData;
    layoutUserData = new QGraphicsLinearLayout (Qt::Horizontal);
    layoutUserData->setContentsMargins (0., 0., 0., 0.);
    layoutUserData->setSpacing(0);

    layoutUserData->addItem(layoutUserName);
    layoutUserData->addItem(layoutPw);
    layout->addItem(layoutUserData);

    m_gprsBusinessLogic->initiateGprsQueries();

    m_contextButton = new MButton("Open Connection");
    layout->addItem(m_contextButton);
    layout->setAlignment(m_contextButton, Qt::AlignHCenter);
    connect(m_contextButton, SIGNAL(released()), this, SLOT(contextReleased()));

    layout->addStretch(100);

    setLayout (layout);
}

QString
gprsWidget::labelText()
{
    QString retval;

    retval +=tr("Access Point Settings");
    return retval;
}

QString
gprsWidget::apnText()
{
    QString retval;
    QString tmp;
    tmp = m_gprsBusinessLogic->GprsApnName ();
    if ((tmp.isEmpty () == false) &&
        (tmp.compare("emptystring", Qt::CaseSensitive) != 0))
    {
        //% "GprsApnName"
        retval += tmp;
    }
    return retval;
}

QString
gprsWidget::apnUserText()
{
    QString retval;
    QString tmp;
    tmp = m_gprsBusinessLogic->GprsUser ();
    if ((tmp.isEmpty () == false) &&
        (tmp.compare("emptystring", Qt::CaseSensitive) != 0))
    {
        retval += tmp;
    }
    return retval;
}

QString
gprsWidget::apnPasswdText()
{
    QString retval;
    QString tmp;
    tmp = m_gprsBusinessLogic->GprsPasswd ();
    if ((tmp.isEmpty () == false) &&
        (tmp.compare("emptystring", Qt::CaseSensitive) != 0))
    {
        retval += tmp;
    }
    return retval;
}

QString
gprsWidget::apnActiveText()
{
    QString retval;
    QString tmp;
    tmp = m_gprsBusinessLogic->GprsActive ();
    if (tmp.isEmpty () == false) {
        if (tmp=="false") {
            retval=tr("Open Connection");
            contextActive=false;
        }
        else {
            retval=tr("Close Connection");
            contextActive=true;
        }
    }

return retval;
}


void
gprsWidget::refresh ()
{
    m_InfoLabel->setText (labelText());
    m_apnBox->setText (apnText());
    m_userBox->setText (apnUserText());
    m_passwdBox->setText (apnPasswdText());
    m_contextButton->setText (apnActiveText());
    m_mutex.lock();
    int operIndx = getOperatorIndex();
    int indx = m_combo->currentIndex();
    if ((operIndx > 0) &&
        (operIndx != indx)) {
        m_refresh_update = false;
    }
    m_mutex.unlock();
    if (operIndx > 0) {
        m_combo->setCurrentIndex(operIndx);
    }
/*
    if ((operIndx > 0) &&
        (operIndx != indx)) {
        m_combo->setCurrentIndex(operIndx);
    }
*/
}

void
gprsWidget::retranslateUi ()
{
    //refresh ();
}

void 
gprsWidget::contextReleased ()
{
    if (contextActive)
    {
         m_contextButton->setText (tr("Open Connection"));
         m_gprsBusinessLogic->setProperty("Active",false);
         contextActive=false;
         m_apnBox->setReadOnly(false);
         m_userBox->setReadOnly(false);
         m_passwdBox->setReadOnly(false);
    }
    else
    {
         m_contextButton->setText (tr("Close Connection"));
         m_gprsBusinessLogic->setProperty("Active",true);
         contextActive=true;
         m_apnBox->setReadOnly(true);
         m_userBox->setReadOnly(true);
         m_passwdBox->setReadOnly(true);
    }
}

void
gprsWidget::setApn()
{
    QString apnName;
    apnName=m_apnBox->text();
    // workaround as empty string set will fail
    if (apnName.isEmpty() == true) {
        apnName.append("emptystring");
    }
    SYS_DEBUG ("setApn");
    //printf("setApn: %s\n", SYS_STR (apnName));
    m_gprsBusinessLogic->setProperty("AccessPointName", apnName);
}

void
gprsWidget::setUser()
{
    QString userName;
    userName=m_userBox->text();
    // workaround as empty string set will fail
    if (userName.isEmpty() == true) {
        userName.append("emptystring");
    }
    SYS_DEBUG ("setUserName");
    m_gprsBusinessLogic->setProperty("Username",userName);
}

void
gprsWidget::setPassword()
{
    QString password;
    password=m_passwdBox->text();
    // workaround as empty string set will fail
    if (password.isEmpty() == true) {
        password.append("emptystring");
    }
    SYS_DEBUG ("setPassword");
    m_gprsBusinessLogic->setProperty("Password",password);
}

void gprsWidget::comboIndexChanged(int index_param) {
    t_operator_data *data;

    // index_param = 0 is special "custom data" case.
    // when that is selected from combobox, 
    // edit fields are not updated from previous values
    m_mutex.lock();
    if ((index_param > 0) &&
        (index_param < m_operator_data_list->count)) {
        // update fields if all of them are currently empty even if the update is otherwise disabled
        // in this way we will setup operator specific settings on first launch but will not overwrite changes user has possible made
        // at the time when widget is started
        if (((apnText().isEmpty() == true) && 
             (apnUserText().isEmpty() == true) &&
             (apnPasswdText().isEmpty() == true)) ||
            (m_refresh_update == true)) {
	    data   = m_operator_data_list->array[index_param];
	    if (data != NULL) {
	        if (data->apn_name != NULL) {
	            QString         op(data->apn_name);
	            m_apnBox->setText(op);
	        }
	        if (data->username != NULL) {
	            QString         username(data->username);
	            m_userBox->setText(username);
	        }
	        if (data->password != NULL) {
	            QString         pw(data->password);
	            m_passwdBox->setText(pw);
	       }
	    }
	}
        m_refresh_update = true;
    }
    m_mutex.unlock();
}


