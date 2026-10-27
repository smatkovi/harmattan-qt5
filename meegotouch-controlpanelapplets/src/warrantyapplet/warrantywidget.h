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
#ifndef WARRANTYWIDGET_H
#define WARRANTYWIDGET_H

#include <QPointer>
#include <dcpstylablewidget.h>
#include "warrantybusinesslogic.h"

class MLinearLayoutPolicy;
class MLabel;

class WarrantyWidget : public DcpStylableWidget
{
    Q_OBJECT

public:
    WarrantyWidget (
            WarrantyBusinessLogic  *warrantyBusinessLogic, 
            QGraphicsWidget        *parent = 0);
    ~WarrantyWidget ();

    void createContent ();

protected:
    void retranslateUi ();

private:
    void addHeaderContainer (MLinearLayoutPolicy *mainLayout);
    void addLabelContainer (
            MLinearLayoutPolicy *mainLayout,
            MLabel              *label1,
            MLabel              *label2);
    void addStretcher (
            MLinearLayoutPolicy *mainLayout,
            const QString       &styleName);
    void initialize ();

private:
    QString                            m_warrantyText;
    bool                               m_warrantyTimer;
    QPointer<WarrantyBusinessLogic>    m_WarrantyBusinessLogic;
    MLabel                            *m_TitleLabel;
    MLabel                            *m_labelExpiration;
    MLabel                            *m_labelTerms;
};

#endif

