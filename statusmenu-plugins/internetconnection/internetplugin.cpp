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
#include <QDBusMessage>
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
    QDBusMessage call = QDBusMessage::createMethodCall(NmService, path,
        "org.freedesktop.DBus.Properties", "Get");
    call << interface << name;
    QDBusReply<QDBusVariant> reply = QDBusConnection::systemBus().call(call);
    return reply.isValid() ? reply.value().variant() : QVariant();
}
}

InternetPlugin::InternetPlugin() : container(0), item(0), wlan(0), menu(0), active(false)
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

    // NetworkManager is followed while the menu shows the row only
    connect(item, SIGNAL(displayEntered()), SLOT(shown()));
    connect(item, SIGNAL(displayExited()), SLOT(hidden()));
    return true;
}

void InternetPlugin::shown()
{
    if (active)
        return;
    active = true;
    QDBusConnection::systemBus().connect(NmService, NmPath, "org.freedesktop.DBus.Properties",
        "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
    update();
}

void InternetPlugin::hidden()
{
    if (!active)
        return;
    active = false;
    QDBusConnection::systemBus().disconnect(NmService, NmPath, "org.freedesktop.DBus.Properties",
        "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
}

QGraphicsWidget *InternetPlugin::widget()
{
    return container;
}

void InternetPlugin::propertiesChanged(const QString &interface, const QVariantMap &changed,
                                       const QStringList &)
{
    if (interface != NmInterface)
        return;
    // the switch from the signal itself; the connection's name and type
    // are asked for when it changes
    if (changed.contains("WirelessEnabled"))
        showWlan(changed.value("WirelessEnabled"));
    if (changed.contains("PrimaryConnection"))
        showConnection(qvariant_cast<QDBusObjectPath>(changed.value("PrimaryConnection")));
}

void InternetPlugin::update()
{
    QVariant enabled = nmProperty(NmPath, NmInterface, "WirelessEnabled");
    if (!showWlan(enabled))
        return;
    showConnection(qvariant_cast<QDBusObjectPath>(
        nmProperty(NmPath, NmInterface, "PrimaryConnection")));
}

bool InternetPlugin::showWlan(const QVariant &enabled)
{
    if (!enabled.isValid()) {
        item->setSubtitle("No connection manager");
        wlan->setVisible(false);
        return false;
    }
    wlan->setVisible(true);
    wlan->blockSignals(true);
    wlan->setChecked(enabled.toBool());
    wlan->blockSignals(false);
    return true;
}

void InternetPlugin::showConnection(const QDBusObjectPath &primary)
{
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
    QDBusMessage call = QDBusMessage::createMethodCall(NmService, NmPath,
        "org.freedesktop.DBus.Properties", "Set");
    call << QString(NmInterface) << QString("WirelessEnabled") << QVariant::fromValue(QDBusVariant(on));
    QDBusConnection::systemBus().call(call);
}

void InternetPlugin::openSettings()
{
    // the connection settings of the desktop the session has, if any
    if (QProcess::startDetached("nm-connection-editor") && menu)
        menu->hideStatusIndicatorMenu();
}
