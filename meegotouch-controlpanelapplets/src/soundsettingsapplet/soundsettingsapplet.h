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
#ifndef _SOUND_SETTINGS_APPLET_H_
#define _SOUND_SETTINGS_APPLET_H_

#include <QStack>
#include "dcpappletif.h"

class AlertTone;
class AlertToneToplevel;
class MAction;

class Q_DECL_EXPORT SoundSettingsApplet : public QObject, public DcpAppletIf
{
Q_OBJECT
Q_INTERFACES (DcpAppletIf)
#if QT_VERSION >= 0x050000
    // Qt 5's plugins say what they are in the class (Q_EXPORT_PLUGIN2 is gone)
    Q_PLUGIN_METADATA(IID "com.nokia.m.core.DcpAppletIf/1.0")
#endif

public:
    SoundSettingsApplet ();
    ~SoundSettingsApplet ();

    static const int AlertToneAppletWidget_id = 0;
    static const int AlertToneBrowser_id = 1;

    virtual void init ();
    virtual QString title () const;

    virtual DcpBrief* constructBrief (int partId);
    virtual DcpStylableWidget *constructStylableWidget (int widgetId);

    // deprecated
    virtual QVector<MAction *> viewMenuItems ();

public slots:
    void toplevelDestroyed (QObject *goner);

    private slots:
        void showWidget (int widgetId);

    private:
        QStack<DcpStylableWidget *>     m_stack;
        QList<AlertTone *>              m_alertTones;

#ifdef UNIT_TEST
    friend class Ut_SoundSettingsAppletTests;
    friend class Ut_AlertToneAppletWidgetTests;
#endif
};

#endif /* !_SOUND_SETTINGS_APPLET_H_ */
