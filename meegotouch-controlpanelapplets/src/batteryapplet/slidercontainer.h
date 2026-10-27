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
#ifndef SLIDERCONTAINER_H
#define SLIDERCONTAINER_H

#include <QObject>
#include <MWidgetController>

class MButton;
class MLabel;
class MSlider;
class MLinearLayoutPolicy;

/*!
 * The original layout guide specified these widgets to be put in a single
 * container, so the original SliderContainer was inherited from MWidgetController. The
 * layout guide was modified, these widgets should go into two separate
 * containers now. So this class has been modified and the two separate
 * container can be accessed through sliderContainer() and labelContainer()
 * methods.
 */
class SliderContainer : public MWidgetController
{
Q_OBJECT

public:
    SliderContainer (MWidget *parent = 0);
    ~SliderContainer ();

public slots:
    void initSlider (const QStringList &values);
    void updateSlider (const int value);

private slots:
    void sliderValueChanged (int value);

signals:
    void PSMThresholdValueChanged (int);

private:
    void updateSliderValueLabel (int index);
    void createWidgets (MWidget *parent);

private:
    MSlider               *m_PSMSlider;
    MLabel                *m_AutoPSMLabel;
    MLabel                *m_PsmValueLabel;
    QStringList            m_SliderValues;
    MLinearLayoutPolicy   *m_LayoutPolicy;
    bool                   m_SliderExists;

friend class Ut_SliderContainer;
};

#endif // SLIDERCONTAINER_H
