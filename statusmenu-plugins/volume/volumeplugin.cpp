/*
 * The status indicator menu's volume (see volumeplugin.h).
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#include "volumeplugin.h"
#include "pulsevolume.h"

#include <MImageWidget>
#include <MSlider>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>

VolumePlugin::VolumePlugin() : container(0), icon(0), slider(0), pulse(0)
{
}

void VolumePlugin::setStatusIndicatorMenuInterface(MStatusIndicatorMenuInterface &)
{
}

bool VolumePlugin::initialize(const QString &)
{
    container = new QGraphicsWidget;
    container->setObjectName("StatusIndicatorMenuVolume");
    // one row high, as the list items below it
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal, container);
    layout->setContentsMargins(0, 0, 0, 0);

    icon = new MImageWidget("icon-m-toolbar-volume-white");
    icon->setObjectName("StatusIndicatorMenuVolumeIcon");
    // where the rows below have theirs
    icon->setStyleName("CommonMainIcon");
    layout->addItem(icon);
    layout->setAlignment(icon, Qt::AlignVCenter);

    slider = new MSlider;
    slider->setObjectName("StatusIndicatorMenuVolumeSlider");
    slider->setStyleName("CommonSliderInverted");
    slider->setRange(0, 100);
    slider->setMinLabelVisible(false);
    slider->setMaxLabelVisible(false);
    slider->setHandleLabelVisible(false);
    layout->addItem(slider);
    layout->setStretchFactor(slider, 1);
    layout->setAlignment(slider, Qt::AlignVCenter);

    pulse = new PulseVolume(this);
    connect(pulse, SIGNAL(volumeChanged(int, bool)), SLOT(volumeChanged(int, bool)));
    connect(slider, SIGNAL(valueChanged(int)), SLOT(sliderMoved(int)));
    // PulseAudio is followed while the menu shows the row only
    connect(slider, SIGNAL(displayEntered()), SLOT(shown()));
    connect(slider, SIGNAL(displayExited()), SLOT(hidden()));
    return true;
}

QGraphicsWidget *VolumePlugin::widget()
{
    return container;
}

void VolumePlugin::volumeChanged(int percent, bool muted)
{
    // from PulseAudio: show it without setting it back
    slider->blockSignals(true);
    slider->setValue(muted ? 0 : percent);
    slider->blockSignals(false);
    icon->setImage(muted || percent == 0 ? "icon-m-toolbar-volume-off-white" : "icon-m-toolbar-volume-white");
}

void VolumePlugin::sliderMoved(int value)
{
    pulse->setVolume(value);
    icon->setImage(value == 0 ? "icon-m-toolbar-volume-off-white" : "icon-m-toolbar-volume-white");
}

void VolumePlugin::shown()
{
    pulse->setActive(true);
}

void VolumePlugin::hidden()
{
    pulse->setActive(false);
}
