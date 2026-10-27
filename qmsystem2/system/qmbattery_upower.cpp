/*!
 * @file qmbattery_upower.cpp
 * @brief QmBattery without the N9's battery management (BME): the battery
 * as UPower sees it, or the kernel's power supply class when there is no
 * UPower.

   <p>
   This file is part of SystemSW QtAPI.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with SystemSW QtAPI.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */
#include "qmbattery.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTimer>
#include <QVariantMap>

#define UPOWER_SERVICE "org.freedesktop.UPower"
#define UPOWER_PATH "/org/freedesktop/UPower"
#define UPOWER_DISPLAY_DEVICE "/org/freedesktop/UPower/devices/DisplayDevice"
#define UPOWER_DEVICE_IF "org.freedesktop.UPower.Device"
#define PROPERTIES_IF "org.freedesktop.DBus.Properties"
#define POWER_SUPPLY_DIR "/sys/class/power_supply"

// what the N9's status bar shows
static const int MaxBars = 8;
// the levels at which the N9 warned
static const int LowPct = 15;
static const int CriticalPct = 5;
static const int EmptyPct = 2;
// how often the kernel's values are read when there is no UPower to tell
static const int PollInterval = 30000;

namespace MeeGo
{
    class QmBatteryPrivate : public QObject
    {
        Q_OBJECT

    public:
        QmBatteryPrivate(QmBattery *battery);

        // the values, as BME gave them
        int percentage;         // -1: not known
        bool present;
        bool charging;
        bool full;
        bool charger;           // external power connected
        int voltage;            // mV
        int current;            // mA, negative when discharging
        int capacity;           // mAh, now
        int nominalCapacity;    // mAh, design
        int timeToEmpty;        // s
        int timeToFull;         // s

        QmBattery::BatteryState batteryState() const;
        QmBattery::Level level() const;
        int bars() const;

    public Q_SLOTS:
        void refresh();
        void upowerPropertiesChanged(const QString &interface,
                                     const QVariantMap &changed,
                                     const QStringList &invalidated);

    private:
        bool readUPower();
        bool readPowerSupply();
        void emitChanges();

        QmBattery *q;
        bool useUPower;
        QTimer pollTimer;

        // what was signalled last
        int lastPercentage;
        bool lastCharging;
        bool lastCharger;
        QmBattery::BatteryState lastState;
        QmBattery::Level lastLevel;
    };

    // the first line of a power supply attribute, empty if it is not there
    static QString attribute(const QString &dir, const char *name)
    {
        QFile f(dir + "/" + name);
        if (!f.open(QIODevice::ReadOnly))
            return QString();
        return QString::fromLatin1(f.readLine()).trimmed();
    }

    QmBatteryPrivate::QmBatteryPrivate(QmBattery *battery)
        : percentage(-1), present(false), charging(false), full(false),
          charger(false), voltage(0), current(0), capacity(0),
          nominalCapacity(0), timeToEmpty(0), timeToFull(0),
          q(battery), useUPower(false),
          lastPercentage(-2), lastCharging(false), lastCharger(false),
          lastState(QmBattery::StateError), lastLevel(QmBattery::LevelFull)
    {
        QDBusConnection bus = QDBusConnection::systemBus();
        if (bus.isConnected() && bus.interface()
            && bus.interface()->isServiceRegistered(UPOWER_SERVICE)) {
            useUPower = true;
            bus.connect(UPOWER_SERVICE, UPOWER_DISPLAY_DEVICE, PROPERTIES_IF,
                        "PropertiesChanged", this,
                        SLOT(upowerPropertiesChanged(QString,QVariantMap,QStringList)));
            // the line power device changes without the display device
            bus.connect(UPOWER_SERVICE, UPOWER_PATH, PROPERTIES_IF,
                        "PropertiesChanged", this,
                        SLOT(upowerPropertiesChanged(QString,QVariantMap,QStringList)));
        } else {
            // the kernel does not tell, it is asked from time to time
            connect(&pollTimer, SIGNAL(timeout()), SLOT(refresh()));
            pollTimer.start(PollInterval);
        }
        refresh();
    }

    bool QmBatteryPrivate::readUPower()
    {
        QDBusInterface dev(UPOWER_SERVICE, UPOWER_DISPLAY_DEVICE, PROPERTIES_IF,
                           QDBusConnection::systemBus());
        QDBusReply<QVariantMap> r = dev.call("GetAll", UPOWER_DEVICE_IF);
        if (!r.isValid())
            return false;
        const QVariantMap p = r.value();
        present = p.value("IsPresent").toBool();
        percentage = present ? qRound(p.value("Percentage").toDouble()) : -1;
        // 1 charging, 2 discharging, 3 empty, 4 fully charged,
        // 5 pending charge, 6 pending discharge
        const uint state = p.value("State").toUInt();
        charging = state == 1 || state == 5;
        full = state == 4;
        timeToEmpty = p.value("TimeToEmpty").toLongLong();
        timeToFull = p.value("TimeToFull").toLongLong();
        const double volts = p.value("Voltage").toDouble();
        voltage = qRound(volts * 1000);
        if (volts > 0) {
            // Wh and W to mAh and mA at this voltage
            capacity = qRound(p.value("Energy").toDouble() / volts * 1000);
            nominalCapacity = qRound(p.value("EnergyFullDesign").toDouble() / volts * 1000);
            const int rate = qRound(p.value("EnergyRate").toDouble() / volts * 1000);
            current = charging ? rate : -rate;
        }

        // external power: UPower's OnBattery, or charging at all
        QDBusInterface up(UPOWER_SERVICE, UPOWER_PATH, PROPERTIES_IF,
                          QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> onBattery = up.call("Get", UPOWER_SERVICE, "OnBattery");
        charger = onBattery.isValid() ? !onBattery.value().variant().toBool()
                                      : (charging || full);
        return true;
    }

    bool QmBatteryPrivate::readPowerSupply()
    {
        QDir dir(POWER_SUPPLY_DIR);
        bool found = false;
        charger = false;
        foreach (const QString &name, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            const QString d = dir.filePath(name);
            const QString type = attribute(d, "type");
            if (type == "Mains" || type == "USB" || type.startsWith("USB_")) {
                if (attribute(d, "online") == "1")
                    charger = true;
                continue;
            }
            if (type != "Battery" || found)
                continue;
            // not a mouse's, a headset's or a connected phone's battery
            // (scope Device), and one that tells its charge
            if (attribute(d, "scope") == "Device" || attribute(d, "capacity").isEmpty())
                continue;
            const QString p = attribute(d, "present");
            present = p.isEmpty() || p == "1";
            if (!present)
                continue;
            found = true;
            const QString status = attribute(d, "status");
            charging = status == "Charging";
            full = status == "Full";
            percentage = attribute(d, "capacity").toInt();
            // µV, µA, µAh, s
            voltage = attribute(d, "voltage_now").toLongLong() / 1000;
            current = attribute(d, "current_now").toLongLong() / 1000;
            if (current > 0 && status == "Discharging")
                // drivers differ in the sign
                current = -current;
            capacity = attribute(d, "charge_now").toLongLong() / 1000;
            nominalCapacity = attribute(d, "charge_full_design").toLongLong() / 1000;
            timeToEmpty = attribute(d, "time_to_empty_now").toInt();
            timeToFull = attribute(d, "time_to_full_now").toInt();
        }
        if (!found) {
            present = false;
            percentage = -1;
        }
        if (charging || full)
            charger = true;
        return found;
    }

    QmBattery::BatteryState QmBatteryPrivate::batteryState() const
    {
        if (percentage < 0)
            return QmBattery::StateError;
        if (full || percentage >= 100)
            return QmBattery::StateFull;
        if (percentage <= EmptyPct && !charging)
            return QmBattery::StateEmpty;
        if (percentage <= LowPct)
            return QmBattery::StateLow;
        return QmBattery::StateOK;
    }

    QmBattery::Level QmBatteryPrivate::level() const
    {
        if (percentage >= 0 && percentage <= CriticalPct)
            return QmBattery::LevelCritical;
        if (percentage >= 0 && percentage <= LowPct)
            return QmBattery::LevelLow;
        return QmBattery::LevelFull;
    }

    int QmBatteryPrivate::bars() const
    {
        if (percentage <= 0)
            return 0;
        return qBound(1, (percentage * MaxBars + 99) / 100, MaxBars);
    }

    void QmBatteryPrivate::refresh()
    {
        if (!(useUPower && readUPower()))
            readPowerSupply();
        emitChanges();
    }

    void QmBatteryPrivate::upowerPropertiesChanged(const QString &, const QVariantMap &,
                                                   const QStringList &)
    {
        refresh();
    }

    void QmBatteryPrivate::emitChanges()
    {
        if (percentage != lastPercentage) {
            lastPercentage = percentage;
            emit q->batteryRemainingCapacityChanged(qMax(percentage, 0), bars());
            emit q->batteryEnergyLevelChanged(qMax(percentage, 0));
        }
        if (charger != lastCharger) {
            lastCharger = charger;
            emit q->chargerEvent(charger ? QmBattery::Wall : QmBattery::None);
        }
        if (charging != lastCharging) {
            lastCharging = charging;
            QmBattery::ChargingState s = charging ? QmBattery::StateCharging
                                                  : QmBattery::StateNotCharging;
            emit q->chargingStateChanged(s);
            emit q->batteryStatusChanged(s);
        }
        const QmBattery::BatteryState state = batteryState();
        if (state != lastState) {
            lastState = state;
            emit q->batteryStateChanged(state);
        }
        const QmBattery::Level l = level();
        if (l != lastLevel) {
            lastLevel = l;
            emit q->batteryLevelChanged(l);
        }
    }

    QmBattery::QmBattery(QObject *parent)
        : QObject(parent), pimpl_(new QmBatteryPrivate(this))
    {
    }

    QmBattery::~QmBattery()
    {
    }

    int QmBattery::getNominalCapacity() const
    {
        return pimpl_->nominalCapacity;
    }

    QmBattery::BatteryState QmBattery::getBatteryState() const
    {
        return pimpl_->batteryState();
    }

    int QmBattery::getRemainingCapacitymAh() const
    {
        return pimpl_->capacity;
    }

    int QmBattery::getRemainingCapacityPct() const
    {
        return qMax(pimpl_->percentage, 0);
    }

    int QmBattery::getRemainingCapacityBars() const
    {
        return pimpl_->bars();
    }

    int QmBattery::getMaxBars() const
    {
        return MaxBars;
    }

    int QmBattery::getVoltage() const
    {
        return pimpl_->voltage;
    }

    int QmBattery::getBatteryCurrent() const
    {
        return pimpl_->current;
    }

    int QmBattery::getCumulativeBatteryCurrent() const
    {
        // (BME kept a running sum, nobody here does)
        return 0;
    }

    QmBattery::ChargerType QmBattery::getChargerType() const
    {
        return pimpl_->charger ? QmBattery::Wall : QmBattery::None;
    }

    QmBattery::ChargingState QmBattery::getChargingState() const
    {
        return pimpl_->charging ? QmBattery::StateCharging : QmBattery::StateNotCharging;
    }

    int QmBattery::getRemainingChargingTime() const
    {
        return pimpl_->timeToFull;
    }

    bool QmBattery::startCurrentMeasurement(Period)
    {
        return false;
    }

    bool QmBattery::stopCurrentMeasurement()
    {
        return false;
    }

    int QmBattery::getAverageTalkCurrent(RemainingTimeMode) const
    {
        return qAbs(pimpl_->current);
    }

    int QmBattery::getAverageActiveCurrent(RemainingTimeMode) const
    {
        return qAbs(pimpl_->current);
    }

    int QmBattery::getRemainingTalkTime(RemainingTimeMode) const
    {
        return pimpl_->timeToEmpty;
    }

    int QmBattery::getRemainingIdleTime(RemainingTimeMode) const
    {
        return pimpl_->timeToEmpty;
    }

    int QmBattery::getRemainingActiveTime(RemainingTimeMode) const
    {
        return pimpl_->timeToEmpty;
    }

    int QmBattery::getAverageIdleCurrent(RemainingTimeMode) const
    {
        return qAbs(pimpl_->current);
    }

    QmBattery::BatteryCondition QmBattery::getBatteryCondition() const
    {
        return QmBattery::ConditionUnknown;
    }

    int QmBattery::getBatteryEnergyLevel() const
    {
        return qMax(pimpl_->percentage, 0);
    }

    QmBattery::Level QmBattery::getLevel() const
    {
        return pimpl_->level();
    }

    QmBattery::State QmBattery::getState() const
    {
        return getChargingState();
    }
}

#include "qmbattery_upower.moc"
