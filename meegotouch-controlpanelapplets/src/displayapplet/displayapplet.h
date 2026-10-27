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
#ifndef DISPLAYAPPLET_H
#define DISPLAYAPPLET_H

#include <DcpAppletIf>
#include <DcpWidget>
#include <DcpBrief>
#include <QObject>

class MAction;
class DisplayWidget;

class Q_DECL_EXPORT DisplayApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES(DcpAppletIf)

public:
    enum {
        DefaultPart,
        BrightnessPart
    };

    virtual void init();
    virtual DcpWidget* constructWidget(int widgetId);
    virtual DcpWidget* pageMain();
    virtual QVector<MAction *> viewMenuItems();
    virtual DcpBrief* constructBrief(int partId);

    virtual int partID (const QString& partStr);

private:
    QPointer<DisplayWidget> m_MainWidget;

    #ifdef UNIT_TEST
    friend class Ut_DisplayApplet;
    friend class Ut_DisplayBrief;
    #endif
};
#endif // DISPLAYAPPLET_H

