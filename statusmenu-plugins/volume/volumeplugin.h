/*
 * The status indicator menu's volume: the speaker icon and a slider for
 * PulseAudio's default output, as the N9 had it at the top of the menu.
 * Written anew for the Qt 5 port (the N9's plugin was not released).
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#ifndef VOLUMEPLUGIN_H
#define VOLUMEPLUGIN_H

#include <MStatusIndicatorMenuExtensionInterface>
#include <QObject>

class MImageWidget;
class MSlider;
class PulseVolume;
class QGraphicsWidget;

class VolumePlugin : public QObject, public MStatusIndicatorMenuExtensionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0")
    Q_INTERFACES(MStatusIndicatorMenuExtensionInterface MApplicationExtensionInterface)

public:
    VolumePlugin();

    virtual void setStatusIndicatorMenuInterface(MStatusIndicatorMenuInterface &menuInterface);
    virtual bool initialize(const QString &interface);
    virtual QGraphicsWidget *widget();

private slots:
    void volumeChanged(int percent, bool muted);
    void sliderMoved(int value);

private:
    QGraphicsWidget *container;
    MImageWidget *icon;
    MSlider *slider;
    PulseVolume *pulse;
};

#endif
