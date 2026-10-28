/*
 * Qt 5 has no primary touch point any more (Qt 4's Qt::TouchPointPrimary,
 * QTouchEvent::TouchPoint::isPrimary()): the first finger of a touch
 * sequence, the one Qt 4 also made mouse events of. The keyboard passes it
 * on with its key events and makes mouse events of it for the extended
 * keys. Kept here: a point the keyboard makes itself carries a flag (Qt 5
 * keeps a touch point's flags with it), a point from Qt is primary when it
 * has the id of the first point of the sequence (Qt's events have a touch
 * device, the keyboard's own have none).
 *
 * This file is part of meego-keyboard (BSD licence, see LICENSE).
 */
#ifndef TOUCHCOMPAT_H
#define TOUCHCOMPAT_H

#include <QTouchEvent>

#if QT_VERSION >= 0x050000
namespace TouchCompat {

const QTouchEvent::TouchPoint::InfoFlag PrimaryFlag = QTouchEvent::TouchPoint::InfoFlag(0x4000);

inline int &primaryId()
{
    static int id = -1;
    return id;
}

//! The first point of a sequence from Qt is its primary one.
inline void noteEvent(const QTouchEvent *event)
{
    if (event->type() == QEvent::TouchBegin && event->device()
        && !event->touchPoints().isEmpty())
        primaryId() = event->touchPoints().first().id();
}

inline bool isPrimary(const QTouchEvent::TouchPoint &tp)
{
    return (tp.flags() & PrimaryFlag) || tp.id() == primaryId();
}

inline void setPrimary(QTouchEvent::TouchPoint &tp, bool primary)
{
    tp.setFlags(primary ? (tp.flags() | PrimaryFlag) : (tp.flags() & ~PrimaryFlag));
}

}
//! The keyboard's own touch events: no device (see above).
#define M_TOUCH_DEVICE static_cast<QTouchDevice *>(0)
#define M_TP_IS_PRIMARY(tp) TouchCompat::isPrimary(tp)
#define M_TP_STATE(tp) (int((tp).state()) & 0x0f)
#else
#define M_TOUCH_DEVICE QTouchEvent::TouchScreen
#define M_TP_IS_PRIMARY(tp) (tp).isPrimary()
#define M_TP_STATE(tp) ((tp).state() & Qt::TouchPointStateMask)
#endif

#endif
