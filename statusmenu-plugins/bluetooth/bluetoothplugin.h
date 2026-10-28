/*
 * The status indicator menu's Bluetooth: on or off (BlueZ's first adapter)
 * and the devices connected, with a switch. The N9's menu had no Bluetooth
 * row; written anew for the Qt 5 port, in the style of its other rows.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#ifndef BLUETOOTHPLUGIN_H
#define BLUETOOTHPLUGIN_H

#include <MStatusIndicatorMenuExtensionInterface>
#include <QObject>
#include <QVariantMap>

class MBasicListItem;
class MButton;
class QGraphicsWidget;

class BluetoothPlugin : public QObject, public MStatusIndicatorMenuExtensionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0")
    Q_INTERFACES(MStatusIndicatorMenuExtensionInterface MApplicationExtensionInterface)

public:
    BluetoothPlugin();

    virtual void setStatusIndicatorMenuInterface(MStatusIndicatorMenuInterface &menuInterface);
    virtual bool initialize(const QString &interface);
    virtual QGraphicsWidget *widget();

private slots:
    void update();
    void toggled(bool on);

private:
    QGraphicsWidget *container;
    MBasicListItem *item;
    MButton *power;
    QString adapter;
};

#endif
