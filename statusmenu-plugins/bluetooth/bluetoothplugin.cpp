/*
 * The status indicator menu's Bluetooth (see bluetoothplugin.h).
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#include "bluetoothplugin.h"
#include "invertedlistitem.h"

#include <MBasicListItem>
#include <MButton>
#include <MImageWidget>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusVariant>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QStringList>

// BlueZ's objects (org.freedesktop.DBus.ObjectManager): path -> interface ->
// properties
typedef QMap<QString, QVariantMap> InterfaceMap;
typedef QMap<QDBusObjectPath, InterfaceMap> ManagedObjects;
Q_DECLARE_METATYPE(InterfaceMap)
Q_DECLARE_METATYPE(ManagedObjects)

namespace {
const char *const BluezService = "org.bluez";
const char *const AdapterInterface = "org.bluez.Adapter1";
const char *const DeviceInterface = "org.bluez.Device1";
}

BluetoothPlugin::BluetoothPlugin() : container(0), item(0), power(0)
{
    qDBusRegisterMetaType<InterfaceMap>();
    qDBusRegisterMetaType<ManagedObjects>();
}

void BluetoothPlugin::setStatusIndicatorMenuInterface(MStatusIndicatorMenuInterface &)
{
}

bool BluetoothPlugin::initialize(const QString &)
{
    container = new QGraphicsWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal, container);
    layout->setContentsMargins(0, 0, 0, 0);

    item = new InvertedListItem;
    item->setObjectName("StatusIndicatorMenuBluetooth");
    item->setTitle("Bluetooth");
    item->imageWidget()->setImage("icon-m-common-bluetooth");
    layout->addItem(item);
    layout->setStretchFactor(item, 1);

    power = new MButton;
    power->setViewType(MButton::switchType);
    power->setStyleName("CommonSwitchInverted");
    power->setCheckable(true);
    layout->addItem(power);
    layout->setAlignment(power, Qt::AlignVCenter);
    connect(power, SIGNAL(toggled(bool)), SLOT(toggled(bool)));

    // any change of an adapter or a device, and adapters coming and going
    QDBusConnection bus = QDBusConnection::systemBus();
    bus.connect(BluezService, QString(), "org.freedesktop.DBus.Properties",
                "PropertiesChanged", this, SLOT(update()));
    bus.connect(BluezService, "/", "org.freedesktop.DBus.ObjectManager",
                "InterfacesAdded", this, SLOT(update()));
    bus.connect(BluezService, "/", "org.freedesktop.DBus.ObjectManager",
                "InterfacesRemoved", this, SLOT(update()));
    update();
    return true;
}

QGraphicsWidget *BluetoothPlugin::widget()
{
    return container;
}

void BluetoothPlugin::update()
{
    QDBusMessage call = QDBusMessage::createMethodCall(BluezService, "/",
        "org.freedesktop.DBus.ObjectManager", "GetManagedObjects");
    QDBusMessage reply = QDBusConnection::systemBus().call(call, QDBus::Block, 2000);
    ManagedObjects objects;
    if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty())
        reply.arguments().at(0).value<QDBusArgument>() >> objects;

    adapter.clear();
    bool powered = false;
    QStringList connected;
    for (ManagedObjects::const_iterator i = objects.constBegin(); i != objects.constEnd(); ++i) {
        if (adapter.isEmpty() && i.value().contains(AdapterInterface)) {
            adapter = i.key().path();
            powered = i.value().value(AdapterInterface).value("Powered").toBool();
        }
        const QVariantMap device = i.value().value(DeviceInterface);
        if (device.value("Connected").toBool())
            connected << device.value("Alias", device.value("Name")).toString();
    }

    power->setVisible(!adapter.isEmpty());
    item->imageWidget()->setImage(powered ? "icon-m-bluetooth" : "icon-m-bluetooth-off");
    power->blockSignals(true);
    power->setChecked(powered);
    power->blockSignals(false);
    if (adapter.isEmpty())
        item->setSubtitle("No Bluetooth");
    else if (!powered)
        item->setSubtitle("Off");
    else if (connected.isEmpty())
        item->setSubtitle("On");
    else
        item->setSubtitle(connected.join(", "));
}

void BluetoothPlugin::toggled(bool on)
{
    if (adapter.isEmpty())
        return;
    QDBusInterface props(BluezService, adapter, "org.freedesktop.DBus.Properties",
                         QDBusConnection::systemBus());
    props.call("Set", QString(AdapterInterface), QString("Powered"),
               QVariant::fromValue(QDBusVariant(on)));
}
