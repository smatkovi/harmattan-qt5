/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MIMPLUGINENABLERWIDGET_H
#define MIMPLUGINENABLERWIDGET_H

#include <QString>
#include <MContainer>

#include "mimonscreenplugins.h"

class MButton;
class MLabel;
class MImSettingsListItem;
class MImageWidget;

//! \internal
//!
//! \brief Widget for enabling specific plugins

class MImPluginEnablerWidget : public MContainer
{
    Q_OBJECT
public:
    //! Create the widget.
    //! \param pluginName The name of the plugin to enable/disable
    explicit MImPluginEnablerWidget(const QString &pluginName,
                                    const QString &displayName,
                                    MImOnScreenPlugins &osp,
                                    MImOnScreenPlugins::SubView subview,
                                    QGraphicsItem *parent = 0);
    virtual ~MImPluginEnablerWidget();

signals:
    void showSettings(const QString& pluginName);

private slots:
    void updateEnabledState();
    void enablePlugin(bool enabled);
    void showSettings();

protected:
    //! \reimp
    void retranslateUi();
    //! \reimp_end
private:
    void setDrillDownEnabled(bool enabled);
    QString pluginName;
    QString displayName;
    MImOnScreenPlugins &onScreenPlugins;
    const MImOnScreenPlugins::SubView subview;
    MButton *enableButton;
    MLabel *pluginTitle;
    MImSettingsListItem *drillDownItem;
    MImageWidget *drillDownArrow;
};

#endif // MIMPLUGINENABLERWIDGET_H
