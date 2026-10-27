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
#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include <DcpStylableWidget>
#include <QString>

class MButton;
class MComboBox;
class MLabel;
class MLayout;
class MLinearLayoutPolicy;
class MWidgetController;
class BatteryBusinessLogic;
class BatteryImage;
class SliderContainer;
class PercentageContainer;
class MSeparator;

class BatteryWidget : public DcpStylableWidget
{
    Q_OBJECT

public:

    typedef enum {
        PSMAutoOff=0,
        PSMAutoOn,
        PSMAutoAutomatic}PowerSaveOpt;

    BatteryWidget (QGraphicsWidget *parent = 0);
    ~BatteryWidget ();
    bool back ();

protected:
    void initWidget();

private slots:
    void remainingBatteryCapacityReceived (const int value);
    void PSMValueReceived (bool PSMEnabled);
    void PSMAutoActivated (int PSMAutoMode);
    void charging(int animation_rate);
    void chargeComplete ();
    void updateUIonPowerSaveModeChange (PowerSaveOpt powerSaveopt);
    void displayEnteredSlot ();
    void displayExitedSlot ();
    
private:
    void addHeaderContainer ();
    void addRemainingCapacityWidget ();
    void addAutoActivationWidget ();
    void addSliderContainer ();
    void addBatteryConditionWidget ();
    MSeparator *addSpacer (
            const QString &styleName, 
            int            index);

    void showSlider (bool show);
    void retranslateUi ();
    void showHideUi ();
    void fromProperBatteryInfo (unsigned int pct = 0);
    bool batteryConditionInfo (QString &shortDescriptionText);

private: 
    BatteryBusinessLogic    *m_logic;
    MLinearLayoutPolicy     *m_MainLayout;
    PercentageContainer     *m_RemainingContainer;
    MWidgetController       *m_ActivationContainer;
    // SliderContainer is not a container any more!
    SliderContainer         *m_SliderContainer;
    MComboBox               *m_PSMAutoCombo;
    BatteryImage            *m_BatteryImage;
    MLabel                  *m_TitleLabel;
    bool                     m_UILocked;
    MSeparator              *spacer;
};

#endif // BATTERYWIDGET_H
