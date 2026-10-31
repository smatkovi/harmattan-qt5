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
#include "qmheartbeat.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
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
// when the kernel's values are read, there being no UPower to tell: once a
// minute, at the minute (between 30 and 90 seconds from the last time),
// when the status bar's clock wakes up anyway
static const unsigned short PollMin = 30;
static const unsigned short PollMax = 90;

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
        void poll();
        void upowerPropertiesChanged(const QString &interface,
                                     const QVariantMap &changed,
                                     const QStringList &invalidated);

    private:
        bool readUPower();
        void fromUPower();
        bool readPowerSupply();
        void emitChanges();
        void startPolling();

        QmBattery *q;
        bool useUPower;
        // the display device's properties, as UPower last told them
        QVariantMap upowerProperties;
        // UPower tells OnBattery (else: external power while charging)
        bool onBatteryKnown;
        QmHeartbeat *heartbeat;
        // (where iphbd is to be asked but is not there)
        QTimer *pollTimer;

        // what was signalled last
        int lastPercentage;
        bool lastCharging;
        bool lastCharger;
        QmBattery::BatteryState lastState;
        QmBattery::Level lastLevel;
    };

    // a D-Bus property of UPower's, invalid if it can't be read
    static QVariant upowerProperty(const char *path, const char *interface, const char *name)
    {
        QDBusMessage call = QDBusMessage::createMethodCall(UPOWER_SERVICE, path, PROPERTIES_IF, "Get");
        call << QString(interface) << QString(name);
        QDBusReply<QDBusVariant> r = QDBusConnection::systemBus().call(call);
        return r.isValid() ? r.value().variant() : QVariant();
    }

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
          q(battery), useUPower(false), onBatteryKnown(false), heartbeat(0), pollTimer(0),
          lastPercentage(-2), lastCharging(false), lastCharger(false),
          lastState(QmBattery::StateError), lastLevel(QmBattery::LevelFull)
    {
        QDBusConnection bus = QDBusConnection::systemBus();
        if (bus.isConnected() && bus.interface()
            && (bus.interface()->isServiceRegistered(UPOWER_SERVICE)
#if QT_VERSION >= 0x050e00
                // (started by the bus on the first call)
                || bus.interface()->activatableServiceNames().value().contains(UPOWER_SERVICE)
#endif
                )) {
            useUPower = true;
            bus.connect(UPOWER_SERVICE, UPOWER_DISPLAY_DEVICE, PROPERTIES_IF,
                        "PropertiesChanged", this,
                        SLOT(upowerPropertiesChanged(QString,QVariantMap,QStringList)));
            // the line power device changes without the display device
            bus.connect(UPOWER_SERVICE, UPOWER_PATH, PROPERTIES_IF,
                        "PropertiesChanged", this,
                        SLOT(upowerPropertiesChanged(QString,QVariantMap,QStringList)));
        }
        refresh();
        if (!useUPower)
            startPolling();
    }

    void QmBatteryPrivate::startPolling()
    {
        // the kernel does not tell, it is asked from time to time; not
        // at all where it has no power supplies to ask about (FreeBSD)
        if (heartbeat || !QDir(POWER_SUPPLY_DIR).exists())
            return;
        heartbeat = new QmHeartbeat(this);
        if (heartbeat->open(QmHeartbeat::SignalNeeded)) {
            connect(heartbeat, SIGNAL(wakeUp(QTime)), SLOT(poll()));
            heartbeat->wait(PollMin, PollMax, QmHeartbeat::DoNotWaitHeartbeat);
        } else {
            pollTimer = new QTimer(this);
            pollTimer->setTimerType(Qt::VeryCoarseTimer);
            connect(pollTimer, SIGNAL(timeout()), SLOT(refresh()));
            pollTimer->start(60000);
        }
    }

    void QmBatteryPrivate::poll()
    {
        refresh();
        heartbeat->wait(PollMin, PollMax, QmHeartbeat::DoNotWaitHeartbeat);
    }

    bool QmBatteryPrivate::readUPower()
    {
        QDBusMessage call = QDBusMessage::createMethodCall(UPOWER_SERVICE, UPOWER_DISPLAY_DEVICE,
                                                           PROPERTIES_IF, "GetAll");
        call << QString(UPOWER_DEVICE_IF);
        QDBusReply<QVariantMap> r = QDBusConnection::systemBus().call(call);
        if (!r.isValid())
            return false;
        upowerProperties = r.value();
        // external power: UPower's OnBattery, or charging at all
        const QVariant onBattery = upowerProperty(UPOWER_PATH, UPOWER_SERVICE, "OnBattery");
        fromUPower();
        onBatteryKnown = onBattery.isValid();
        charger = onBatteryKnown ? !onBattery.toBool() : (charging || full);
        return true;
    }

    void QmBatteryPrivate::fromUPower()
    {
        const QVariantMap &p = upowerProperties;
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
        if (useUPower && !readUPower()) {
            // UPower is gone (or never came): the kernel, then
            useUPower = false;
            startPolling();
        }
        if (!useUPower)
            readPowerSupply();
        emitChanges();
    }

    void QmBatteryPrivate::upowerPropertiesChanged(const QString &interface,
                                                   const QVariantMap &changed,
                                                   const QStringList &invalidated)
    {
        if (!useUPower)
            return;
        // what the signal says, without asking again: UPower tells of each
        // of its refreshes (UpdateTime), most change nothing shown here
        static const char *const used[] = { "IsPresent", "Percentage", "State",
            "TimeToEmpty", "TimeToFull", "Voltage", "Energy", "EnergyFullDesign",
            "EnergyRate" };
        if (!invalidated.isEmpty()) {
            refresh();
            return;
        }
        if (interface == UPOWER_DEVICE_IF) {
            bool relevant = false;
            for (QVariantMap::const_iterator i = changed.constBegin(); i != changed.constEnd(); ++i) {
                upowerProperties.insert(i.key(), i.value());
                for (unsigned u = 0; u < sizeof used / sizeof *used; ++u)
                    relevant |= i.key() == QLatin1String(used[u]);
            }
            if (!relevant)
                return;
            const bool wasCharger = charger;
            fromUPower();
            charger = onBatteryKnown ? wasCharger : (charging || full);
        } else if (interface == UPOWER_SERVICE && changed.contains("OnBattery")) {
            onBatteryKnown = true;
            charger = !changed.value("OnBattery").toBool();
        } else {
            return;
        }
        emitChanges();
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
