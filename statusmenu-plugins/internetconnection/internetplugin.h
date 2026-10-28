/*
 * The status indicator menu's internet connection: the connection in use
 * (NetworkManager's primary one) and a switch for WLAN, as the N9's menu had
 * its connection row. Written anew for the Qt 5 port.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#ifndef INTERNETPLUGIN_H
#define INTERNETPLUGIN_H

#include <MStatusIndicatorMenuExtensionInterface>
#include <QObject>
#include <QVariantMap>

class MBasicListItem;
class MButton;
class QGraphicsWidget;

class InternetPlugin : public QObject, public MStatusIndicatorMenuExtensionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0")
    Q_INTERFACES(MStatusIndicatorMenuExtensionInterface MApplicationExtensionInterface)

public:
    InternetPlugin();

    virtual void setStatusIndicatorMenuInterface(MStatusIndicatorMenuInterface &menuInterface);
    virtual bool initialize(const QString &interface);
    virtual QGraphicsWidget *widget();

private slots:
    void update();
    void propertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &);
    void wlanToggled(bool on);
    void openSettings();

private:
    QGraphicsWidget *container;
    MBasicListItem *item;
    MButton *wlan;
    MStatusIndicatorMenuInterface *menu;
};

#endif
