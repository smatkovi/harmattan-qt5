/*!
 * @file qmheartbeat_noiphb.cpp
 * @brief QmHeartbeat without the N9's IP heartbeat service (iphbd): timers
 * at moments the processes agree on, woken together.

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
#include "qmheartbeat.h"

#include <QDateTime>
#include <QList>
#include <QMutex>
#include <QPointer>
#include <QThread>
#include <QTimer>

namespace MeeGo
{
    /*
     * iphbd woke the processes whose windows (mintime to maxtime) held the
     * same moment together, to wake the device less often. Without it,
     * a wait is placed on the first multiple, on the wall clock, of the
     * largest of iphbd's slots (or of a minute, ten, five seconds, a
     * second) that falls into its window: processes that ask alike pick
     * the same moment without talking to each other (a clock's next minute
     * is the minute for everyone). Within the process, when one wait ends,
     * every other one whose mintime has passed ends with it, as iphbd's
     * did, so their timers don't each wake the process again.
     */
    class QmHeartbeatPrivate : public QObject
    {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmHeartbeat)

    public:
        QmHeartbeatPrivate();
        ~QmHeartbeatPrivate();

        //! Waits until @target, not ending before @earliest (ms since the epoch)
        void schedule(qint64 earliest, qint64 target);
        void cancel();

        bool opened;
        bool signalNeeded;

    private Q_SLOTS:
        void timeout();

    private:
        void wake();

        qint64 earliest;    // ms since the epoch, 0: not waiting
        QTimer timer;
    };

    // every heartbeat of the process, for waking them together
    static QMutex instancesMutex;
    static QList<QmHeartbeatPrivate *> instances;

    // A timer may end a little before the wall clock says it should (the
    // two clocks are read one after the other): this much later, then
    static const qint64 Margin = 5;

    QmHeartbeatPrivate::QmHeartbeatPrivate()
        : opened(false), signalNeeded(false), earliest(0)
    {
        timer.setSingleShot(true);
        // the moment is chosen already; a coarse timer would move it
        timer.setTimerType(Qt::PreciseTimer);
        connect(&timer, SIGNAL(timeout()), SLOT(timeout()));
        QMutexLocker locker(&instancesMutex);
        instances.append(this);
    }

    QmHeartbeatPrivate::~QmHeartbeatPrivate()
    {
        QMutexLocker locker(&instancesMutex);
        instances.removeOne(this);
    }

    void QmHeartbeatPrivate::schedule(qint64 earliestMs, qint64 target)
    {
        earliest = earliestMs;
        const qint64 delay = target - QDateTime::currentMSecsSinceEpoch();
        timer.start(int(qMax<qint64>(0, delay) + Margin));
    }

    void QmHeartbeatPrivate::cancel()
    {
        earliest = 0;
        timer.stop();
    }

    void QmHeartbeatPrivate::timeout()
    {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        QList<QPointer<QmHeartbeatPrivate> > due;
        due.append(this);
        {
            QMutexLocker locker(&instancesMutex);
            foreach (QmHeartbeatPrivate *p, instances)
                if (p != this && p->earliest != 0 && p->earliest <= now
                    && p->thread() == thread())
                    due.append(p);
        }
        // (a woken one may delete others, or wait again)
        foreach (const QPointer<QmHeartbeatPrivate> &p, due)
            if (p)
                p->wake();
    }

    void QmHeartbeatPrivate::wake()
    {
        MEEGO_PUBLIC(QmHeartbeat)
        cancel();
        if (signalNeeded)
            emit pub->wakeUp(QTime::currentTime());
    }

    // iphbd's slots: "global synchronization" when mintime and maxtime are one
    static bool isSlot(unsigned short secs)
    {
        return secs == 30 || secs == 150 || secs == 300 || (secs > 0 && secs % 600 == 0);
    }

    // The first multiple of the largest step that lies in [lo, hi] (ms
    // since the epoch), lo if none does
    static qint64 alignedTarget(qint64 lo, qint64 hi)
    {
        // (each a multiple of the next: the moment of a larger step is
        // also one of every smaller one, a clock's minutes included; iphbd's
        // 2.5 minutes are no minute, they are for its slot only)
        static const int steps[] = { 36000, 7200, 3600, 1800, 600, 300,
                                     60, 30, 10, 5, 1 };
        for (unsigned i = 0; i < sizeof steps / sizeof *steps; ++i) {
            const qint64 step = steps[i] * qint64(1000);
            const qint64 t = (lo + step - 1) / step * step;
            if (t <= hi)
                return t;
        }
        return lo;
    }
}

using namespace MeeGo;

// the wake-up slots iphbd knew, in seconds
const unsigned short QmHeartbeat::WAKEUP_SLOT_30_SEC   = 30;
const unsigned short QmHeartbeat::WAKEUP_SLOT_2_5_MINS = 150;
const unsigned short QmHeartbeat::WAKEUP_SLOT_5_MINS   = 300;
const unsigned short QmHeartbeat::WAKEUP_SLOT_10_MINS  = 600;
const unsigned short QmHeartbeat::WAKEUP_SLOT_30_MINS  = 1800;
const unsigned short QmHeartbeat::WAKEUP_SLOT_1_HOUR   = 3600;
const unsigned short QmHeartbeat::WAKEUP_SLOT_2_HOURS  = 7200;
const unsigned short QmHeartbeat::WAKEUP_SLOT_10_HOURS = 36000;

QmHeartbeat::QmHeartbeat(QObject *parent)
    : QObject(parent)
{
    MEEGO_INITIALIZE(QmHeartbeat)
}

QmHeartbeat::~QmHeartbeat()
{
    MEEGO_UNINITIALIZE(QmHeartbeat);
}

bool QmHeartbeat::open(QmHeartbeat::SignalNeed signalNeed)
{
    MEEGO_PRIVATE(QmHeartbeat)
    priv->opened = true;
    priv->signalNeeded = signalNeed == SignalNeeded;
    return true;
}

void QmHeartbeat::close(void)
{
    MEEGO_PRIVATE(QmHeartbeat)
    priv->cancel();
    priv->opened = false;
}

int QmHeartbeat::getFD()
{
    // no iphbd socket to watch
    return -1;
}

QTime QmHeartbeat::wait(unsigned short mintime, unsigned short maxtime,
                        QmHeartbeat::WaitMode wait)
{
    MEEGO_PRIVATE(QmHeartbeat)
    if (!priv->opened)
        return QTime();
    if (mintime == 0 && maxtime == 0) {
        // (libiphb's way of cancelling)
        priv->cancel();
        return QTime();
    }

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const bool slot = mintime == maxtime && isSlot(mintime);
    qint64 lo, hi;
    if (slot) {
        // the slot's next moment, whenever that is
        lo = now + 1;
        hi = now + mintime * qint64(1000);
    } else {
        // whole seconds from this second, as iphbd counted them: a clock
        // that asks for the seconds to its next minute gets the minute
        const qint64 second = now - now % 1000;
        lo = qMax(now, second + mintime * qint64(1000));
        hi = qMax(lo, second + maxtime * qint64(1000));
    }
    const qint64 target = alignedTarget(lo, hi);

    if (wait == WaitHeartbeat) {
        priv->cancel();
        QThread::msleep(qMax<qint64>(0, target - now) + Margin);
        return QTime::currentTime();
    }
    // (a slot's waiter is woken at its moment only, the others also with
    // another one once their mintime has passed)
    priv->schedule(slot ? target : lo, target);
    return QDateTime::fromMSecsSinceEpoch(target).time();
}

bool QmHeartbeat::IWokeUp(void)
{
    MEEGO_PRIVATE(QmHeartbeat)
    // as libiphb's: the pending wake-up is cancelled
    priv->cancel();
    return true;
}

#include "qmheartbeat_noiphb.moc"
