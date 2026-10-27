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
#ifndef ABOUTWIDGET_H
#define ABOUTWIDGET_H

#include <QPointer>
#include <DcpStylableWidget>

#include "aboutbusinesslogic.h"

class MLabel;
class QGraphicsGridLayout;
class MImageWidget;
class ContentWidget;
class CertsWidget;

class AboutWidget : public DcpStylableWidget
{
    Q_OBJECT

public:
    AboutWidget (QGraphicsWidget *parent = 0);
    ~AboutWidget ();

    void createContent ();

private:
    void initialize ();
    void addHeaderContainer ();
    void addLogoContainer ();
    void addNamesContainer ();
    void addVersionContainer ();
    void addWiFiMACContainer ();
    void addIpContainer (int place = -1);
    void addBtMACContainer ();
    void addIMEIContainer ();
    void addCertsContainer ();
    void addBarcodeContainer ();
    void addLicenseLabelContainer ();
    void addStretcher (const QString &styleName);
#ifdef MEEGO
    void addNamesLabelContainer ();
#endif

private slots:
    void gotInfo (AboutBusinessLogic::requestType type,
                  QVariant value);
    void linkActivated (const QString &link);
    void eggs (bool on);

private:
    QGraphicsGridLayout            *m_layout;
    QPointer<AboutBusinessLogic>    m_AboutBusinessLogic;
    MLabel                         *m_TitleLabel;
    ContentWidget                  *m_Version;
    ContentWidget                  *m_ProductName;
    ContentWidget                  *m_WiFi;
    ContentWidget                  *m_Ip;
    ContentWidget                  *m_Bt;
    ContentWidget                  *m_IMEI;
    MLabel                         *m_LicenseLabel;
    MImageWidget                   *m_imgBarcode;
    CertsWidget                    *m_imgCerts;
#ifdef MEEGO
    MLabel                         *m_NamesLabel;
#endif

    int                             m_currentRow;
    int                             m_certsRow;
    int                             m_logoRow;

#ifdef UNIT_TEST
    friend class Ut_AboutApplet;
#endif
};

#endif
