/*
 * The status indicator menu's internet connection (see internetplugin.h).
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#include "internetplugin.h"
#include "invertedlistitem.h"

#include <MBasicListItem>
#include <MButton>
#include <MImageWidget>
#include <mstatusindicatormenuinterface.h>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QProcess>

namespace {
const char *const NmService = "org.freedesktop.NetworkManager";
const char *const NmPath = "/org/freedesktop/NetworkManager";
const char *const NmInterface = "org.freedesktop.NetworkManager";
const char *const ActiveInterface = "org.freedesktop.NetworkManager.Connection.Active";

QVariant nmProperty(const QString &path, const QString &interface, const QString &name)
{
    QDBusInterface props(NmService, path, "org.freedesktop.DBus.Properties",
                         QDBusConnection::systemBus());
    QDBusReply<QDBusVariant> reply = props.call("Get", interface, name);
    return reply.isValid() ? reply.value().variant() : QVariant();
}
}

InternetPlugin::InternetPlugin() : container(0), item(0), wlan(0), menu(0)
{
}

void InternetPlugin::setStatusIndicatorMenuInterface(MStatusIndicatorMenuInterface &menuInterface)
{
    menu = &menuInterface;
}

bool InternetPlugin::initialize(const QString &)
{
    container = new QGraphicsWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal, container);
    layout->setContentsMargins(0, 0, 0, 0);

    item = new InvertedListItem;
    item->setObjectName("StatusIndicatorMenuInternetConnection");
    item->setTitle("Internet connection");
    item->imageWidget()->setImage("icon-m-common-wlan");
    layout->addItem(item);
    layout->setStretchFactor(item, 1);
    connect(item, SIGNAL(clicked()), SLOT(openSettings()));

    wlan = new MButton;
    wlan->setViewType(MButton::switchType);
    wlan->setStyleName("CommonSwitchInverted");
    wlan->setCheckable(true);
    layout->addItem(wlan);
    layout->setAlignment(wlan, Qt::AlignVCenter);
    connect(wlan, SIGNAL(toggled(bool)), SLOT(wlanToggled(bool)));

    QDBusConnection::systemBus().connect(NmService, NmPath, "org.freedesktop.DBus.Properties",
        "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
    update();
    return true;
}

QGraphicsWidget *InternetPlugin::widget()
{
    return container;
}

void InternetPlugin::propertiesChanged(const QString &interface, const QVariantMap &changed,
                                       const QStringList &)
{
    if (interface == NmInterface &&
        (changed.contains("PrimaryConnection") || changed.contains("WirelessEnabled") ||
         changed.contains("State")))
        update();
}

void InternetPlugin::update()
{
    QVariant enabled = nmProperty(NmPath, NmInterface, "WirelessEnabled");
    if (!enabled.isValid()) {
        item->setSubtitle("No connection manager");
        wlan->setVisible(false);
        return;
    }
    wlan->setVisible(true);
    wlan->blockSignals(true);
    wlan->setChecked(enabled.toBool());
    wlan->blockSignals(false);

    QDBusObjectPath primary = qvariant_cast<QDBusObjectPath>(
        nmProperty(NmPath, NmInterface, "PrimaryConnection"));
    QString name, type;
    if (!primary.path().isEmpty() && primary.path() != "/") {
        name = nmProperty(primary.path(), ActiveInterface, "Id").toString();
        type = nmProperty(primary.path(), ActiveInterface, "Type").toString();
    }
    item->setSubtitle(name.isEmpty() ? QString("Offline") : name);
    if (name.isEmpty())
        item->imageWidget()->setImage("icon-m-status-menu-no-internet-connection");
    else if (type == "gsm" || type == "cdma")
        item->imageWidget()->setImage("icon-m-common-packetdata");
    else if (type == "802-3-ethernet")
        item->imageWidget()->setImage("icon-m-bluetooth-lan");
    else
        item->imageWidget()->setImage("icon-m-common-wlan");
}

void InternetPlugin::wlanToggled(bool on)
{
    QDBusInterface props(NmService, NmPath, "org.freedesktop.DBus.Properties",
                         QDBusConnection::systemBus());
    props.call("Set", QString(NmInterface), QString("WirelessEnabled"),
               QVariant::fromValue(QDBusVariant(on)));
}

void InternetPlugin::openSettings()
{
    // the connection settings of the desktop the session has, if any
    if (QProcess::startDetached("nm-connection-editor") && menu)
        menu->hideStatusIndicatorMenu();
}
