/*!
 * @file qmheartbeat_noiphb.cpp
 * @brief QmHeartbeat without the N9's IP heartbeat service (iphbd): a
 * timer at the latest time allowed.

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

#include <QThread>
#include <QTimer>

namespace MeeGo
{
    /*
     * iphbd woke the processes that asked for a time between mintime and
     * maxtime together, to wake the device less often; alone, a process
     * is woken at mintime (the earliest it asked for: a clock's next
     * minute), or at maxtime when it gives no mintime.
     */
    class QmHeartbeatPrivate : public QObject
    {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmHeartbeat)

    public:
        QmHeartbeatPrivate() : opened(false), signalNeeded(false)
        {
            timer.setSingleShot(true);
            connect(&timer, SIGNAL(timeout()), SLOT(timeout()));
        }

        bool opened;
        bool signalNeeded;
        QTimer timer;

    private Q_SLOTS:
        void timeout()
        {
            MEEGO_PUBLIC(QmHeartbeat)
            if (signalNeeded)
                emit pub->wakeUp(QTime::currentTime());
        }
    };
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
    priv->timer.stop();
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
    const int secs = mintime > 0 ? mintime : maxtime;
    if (wait == WaitHeartbeat) {
        QThread::sleep(secs);
        return QTime::currentTime();
    }
    priv->timer.start(secs * 1000);
    return QTime::currentTime().addSecs(secs);
}

bool QmHeartbeat::IWokeUp(void)
{
    return true;
}

#include "qmheartbeat_noiphb.moc"
