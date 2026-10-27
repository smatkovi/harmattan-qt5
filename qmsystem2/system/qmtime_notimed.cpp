/*!
 * @file qmtime_notimed.cpp
 * @brief QmTime without the N9's time daemon (timed): change notifications
 * from the kernel, conversions with the C library.

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
#include "qmtime.h"

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSocketNotifier>
#include <QTimer>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/timerfd.h>
#endif

#define LOCALTIME "/etc/localtime"
#define ZONEINFO "/usr/share/zoneinfo/"

// how often the clock is compared when the kernel can't tell of jumps
static const int PollInterval = 60000;
// a difference to the monotonic clock beyond this is a jump
static const qint64 JumpThreshold = 2000;

namespace MeeGo
{
    /*
     * What timed told on the N9: the system time was set, or the time
     * zone changed. The kernel tells of the first on Linux (a timer that
     * is cancelled when the clock is set), elsewhere the wall clock is
     * compared with the monotonic one from time to time; the time zone
     * is /etc/localtime.
     */
    class QmTimePrivate2 : public QObject
    {
        Q_OBJECT

    public:
        QmTimePrivate2(QObject *parent);
        ~QmTimePrivate2();

    Q_SIGNALS:
        void changed(MeeGo::QmTime::WhatChanged what);
#if F_SUPPORT_DEPRECATED
        void changed_o(MeeGo::QmTimeWhatChanged what);
#endif

    private Q_SLOTS:
        void clockSet();
        void compareClocks();
        void zoneChanged();

    private:
        void armTimer();
        void notify(QmTime::WhatChanged what);

        int timerFd;
        QSocketNotifier *timerNotifier;
        QTimer pollTimer;
        QElapsedTimer monotonic;
        qint64 offset;      // wall clock - monotonic clock, ms
        QFileSystemWatcher watcher;
    };

    QmTimePrivate2::QmTimePrivate2(QObject *parent)
        : QObject(parent), timerFd(-1), timerNotifier(0), offset(0)
    {
#ifdef __linux__
        timerFd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerFd >= 0) {
            armTimer();
            timerNotifier = new QSocketNotifier(timerFd, QSocketNotifier::Read, this);
            connect(timerNotifier, SIGNAL(activated(int)), SLOT(clockSet()));
        }
#endif
        if (timerFd < 0) {
            monotonic.start();
            offset = QDateTime::currentMSecsSinceEpoch() - monotonic.elapsed();
            connect(&pollTimer, SIGNAL(timeout()), SLOT(compareClocks()));
            pollTimer.start(PollInterval);
        }

        // the link itself is replaced, so its directory is watched too
        watcher.addPath(QFileInfo(LOCALTIME).absolutePath());
        if (QFile::exists(LOCALTIME))
            watcher.addPath(LOCALTIME);
        connect(&watcher, SIGNAL(fileChanged(QString)), SLOT(zoneChanged()));
        connect(&watcher, SIGNAL(directoryChanged(QString)), SLOT(zoneChanged()));
    }

    QmTimePrivate2::~QmTimePrivate2()
    {
        if (timerFd >= 0)
            ::close(timerFd);
    }

    void QmTimePrivate2::armTimer()
    {
#ifdef __linux__
        // far in the future, cancelled when the clock is set
        struct itimerspec its;
        memset(&its, 0, sizeof its);
        its.it_value.tv_sec = time(0) + 10 * 365 * 24 * 3600;
        timerfd_settime(timerFd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &its, 0);
#endif
    }

    void QmTimePrivate2::clockSet()
    {
#ifdef __linux__
        uint64_t expirations;
        if (read(timerFd, &expirations, sizeof expirations) < 0 && errno == ECANCELED) {
            armTimer();
            notify(QmTime::TimeChanged);
        }
#endif
    }

    void QmTimePrivate2::compareClocks()
    {
        const qint64 now = QDateTime::currentMSecsSinceEpoch() - monotonic.elapsed();
        if (qAbs(now - offset) > JumpThreshold) {
            offset = now;
            notify(QmTime::TimeChanged);
        }
    }

    void QmTimePrivate2::zoneChanged()
    {
        tzset();
        if (QFile::exists(LOCALTIME) && !watcher.files().contains(LOCALTIME))
            watcher.addPath(LOCALTIME);
        // the local time shown changes with it
        notify(QmTime::TimeChanged);
    }

    void QmTimePrivate2::notify(QmTime::WhatChanged what)
    {
        emit changed(what);
#if F_SUPPORT_DEPRECATED
        emit changed_o(what == QmTime::TimeChanged ? QmTimeTimeChanged
                                                    : QmTimeOnlySettingsChanged);
#endif
    }

    // @tz set in the environment while it lives (for the C library's
    // conversions); the previous one again after
    class TzSwitch
    {
    public:
        TzSwitch(const QString &tz)
        {
            const char *old = getenv("TZ");
            hadOld = old != 0;
            if (hadOld)
                oldTz = old;
            setenv("TZ", (":" + tz).toLocal8Bit().constData(), 1);
            tzset();
        }
        ~TzSwitch()
        {
            if (hadOld)
                setenv("TZ", oldTz.constData(), 1);
            else
                unsetenv("TZ");
            tzset();
        }
    private:
        bool hadOld;
        QByteArray oldTz;
    };
}

using namespace MeeGo;

QmTime::QmTime(QObject *parent) : QObject(parent)
{
    p = new QmTimePrivate2(this);
    connect(p, SIGNAL(changed(MeeGo::QmTime::WhatChanged)),
            this, SIGNAL(timeOrSettingsChanged(MeeGo::QmTime::WhatChanged)));
#if F_SUPPORT_DEPRECATED
    connect(p, SIGNAL(changed_o(MeeGo::QmTimeWhatChanged)),
            this, SIGNAL(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged)));
#endif
}

QmTime::~QmTime()
{
}

bool QmTime::setDisconnectionPolicy(DisconnectionPolicy)
{
    return true;
}

bool QmTime::setSynchronizationPolicy(SettingsSynchronizationPolicy)
{
    return true;
}

bool QmTime::synchronize()
{
    return true;
}

QmTime::AutoSystemTimeStatus QmTime::autoSystemTime()
{
    return AutoSystemTimeUnknown;
}

bool QmTime::setAutoSystemTime(AutoSystemTimeStatus)
{
    return false;
}

QmTime::AutoTimeZoneStatus QmTime::autoTimeZone()
{
    return AutoTimeZoneUnknown;
}

bool QmTime::setAutoTimeZone(AutoTimeZoneStatus)
{
    return false;
}

bool QmTime::setTime(time_t t)
{
    // as root only (timed did it for everyone on the N9)
    struct timeval tv = { t, 0 };
    return settimeofday(&tv, 0) == 0;
}

#if F_SUPPORT_DEPRECATED
bool QmTime::setTime(const QDateTime &time)
{
    return setTime((time_t) time.toTime_t());
}
#endif

bool QmTime::setTimezone(const QString)
{
    // /etc/localtime is root's (timed did it on the N9)
    return false;
}

bool QmTime::getTimezone(QString &tz)
{
    const char *env = getenv("TZ");
    if (env && *env) {
        tz = QString::fromLocal8Bit(*env == ':' ? env + 1 : env);
        return true;
    }
    // the zone /etc/localtime links to (or the one it is a copy of, not known)
    const QString target = QFileInfo(LOCALTIME).canonicalFilePath();
    const int i = target.indexOf(ZONEINFO);
    if (i < 0)
        return false;
    tz = target.mid(i + int(strlen(ZONEINFO)));
    return !tz.isEmpty();
}

bool QmTime::getAutoTimezone(QString &)
{
    return false;
}

time_t QmTime::getAutoTime()
{
    return 0;
}

bool QmTime::isOperatorTimeAccessible(bool &result)
{
    result = false;
    return true;
}

bool QmTime::deviceDefaultTimezone(QString &default_timezone)
{
    return getTimezone(default_timezone);
}

bool QmTime::remoteTime(const QString &tz, time_t t, QDateTime &dt, struct tm *p)
{
    struct tm tm;
    {
        TzSwitch zone(tz);
        if (!localtime_r(&t, &tm))
            return false;
    }
    dt = QDateTime(QDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday),
                   QTime(tm.tm_hour, tm.tm_min, tm.tm_sec));
    if (p)
        *p = tm;
    return true;
}

bool QmTime::localTime(time_t t, QDateTime &dt, struct tm *p)
{
    struct tm tm;
    if (!localtime_r(&t, &tm))
        return false;
    dt = QDateTime(QDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday),
                   QTime(tm.tm_hour, tm.tm_min, tm.tm_sec));
    if (p)
        *p = tm;
    return true;
}

int QmTime::getTimeDiff(time_t t, const QString &location1, const QString &location2)
{
    struct tm tm1, tm2;
    {
        TzSwitch zone(location1);
        localtime_r(&t, &tm1);
    }
    {
        TzSwitch zone(location2);
        localtime_r(&t, &tm2);
    }
    return int(tm1.tm_gmtoff - tm2.tm_gmtoff);
}

#include "qmtime_notimed.moc"
