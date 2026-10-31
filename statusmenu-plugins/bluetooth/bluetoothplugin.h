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
#include <QTimer>
#include <QVariantMap>

class MBasicListItem;
class MButton;
class QDBusPendingCallWatcher;
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
    void shown();
    void hidden();
    void propertiesChanged(const QString &interface, const QVariantMap &changed,
                           const QStringList &invalidated);
    //! Something the row shows changed: asked for again shortly
    void changed();
    void update();
    void gotObjects(QDBusPendingCallWatcher *watcher);
    void toggled(bool on);

private:
    void listen(bool on);

    QGraphicsWidget *container;
    MBasicListItem *item;
    MButton *power;
    QString adapter;
    //! Shown in the menu: following BlueZ's changes
    bool active;
    QTimer updateTimer;
    QDBusPendingCallWatcher *pending;
    bool again;
};

#endif
