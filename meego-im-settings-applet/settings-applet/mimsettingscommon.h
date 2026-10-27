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

#ifndef MIMSETTINGSCOMMON_H
#define MIMSETTINGSCOMMON_H

#include <QtGlobal>

namespace MImSettingsCommon
{
    // Widget Ids for subpages
    const int MainView = 0;
    const int SwypeSettings = 1;

    // Translations
    //% "Text Input"
    const char * const QtnTxtsTextInput(QT_TRID_NOOP("qtn_txts_text_input"));
    //% "Active input method"
    const char * const QtnTxtsActiveInputMethod(QT_TRID_NOOP("qtn_txts_active_input_method"));
    //% "Installed keyboards (%1)"
    const char * const QtnTxtsInstalledKeyboards(QT_TRID_NOOP("qtn_txts_installed_keyboards"));
    //% "Note: you have uninstalled all virtual keyboards"
    const char * const QtnTxtsNoKeyboardsNotification(QT_TRID_NOOP("qtn_txts_no_keyboards_notification"));
    //% "No keyboards installed"
    const char * const QtnTxtsNoKeyboards(QT_TRID_NOOP("qtn_txts_no_keyboards"));
    //% "Enable Swype"
    const char * const QtnTxtsEnableSwype(QT_TRID_NOOP("qtn_txts_enable_swype"));
    //% "Swype Settings"
    const char * const QtnTxtsSwypeSettings(QT_TRID_NOOP("qtn_txts_swype_settings"));

    // Text IDs for strings translated elsewhere:
    // "Settings"
    const char * const QtnCommonSettings("qtn_comm_settings");

    // Style names for various parts of views
    const char * const StyleHeader("CommonHeaderInverted");
    const char * const StyleHeaderDivider("CommonHeaderDividerInverted");
    const char * const StyleSettingsListItem("CommonBasicListItemInverted");
    const char * const StyleImageWidget("CommonComboBoxIconInverted");
    const char * const StyleImageDrillDown("CommonDrillDownIcon");
    const char * const StylePluginContainer("CommonLargePanel");
    const char * const StylePluginHeader("CommonGroupHeaderInverted");
    const char * const StylePluginHeaderContainer("CommonGroupHeaderPanelInverted");
    const char * const StylePluginHeaderDivider("CommonGroupHeaderDividerInverted");
    const char * const StyleAvailableDivider("CommonGroupHeaderDividerInverted");
    const char * const StyleSwitch("CommonRightSwitchInverted");
    const char * const StyleInfoBanner("InformationBanner");
    const char * const StyleTitle("CommonTitleInverted");
    const char * const StyleSubTitle("CommonSubTitleInverted");
    const char * const StyleSingleTitle("CommonSingleTitleInverted");
    const char * const StyleSpinner("CommonLargeSpinnerInverted");
}

#endif // MIMSETTINGSCOMMON_H
