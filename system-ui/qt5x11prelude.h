/*
 * Qt 5 and Xlib in one translation unit (included before everything
 * else, see shared.pri): Qt 5's headers refuse to come after Xlib's
 * Status macro, and its enums break on Xlib's Bool, None and event type
 * macros. Qt's headers come first here, then Xlib, whose Bool becomes a
 * type instead of a macro (QMetaType::Bool in moc's code).
 */
#ifndef SYSTEMUI_QT5X11PRELUDE_H
#define SYSTEMUI_QT5X11PRELUDE_H
#ifdef __cplusplus
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <QtDBus/QtDBus>
// QmSystem2's enums have names Xlib takes as macros (None, ...): first too
#if defined(HAVE_QMSYSTEM) && defined(__has_include)
#if __has_include(<qmbattery.h>)
#include <qmbattery.h>
#include <qmdevicemode.h>
#include <qmdisplaystate.h>
#include <qmlocks.h>
#include <qmsystemstate.h>
#include <qmusbmode.h>
#include <qmtime.h>
#include <qmheartbeat.h>
#endif
#endif
#include <X11/Xlib.h>
#undef Bool
typedef int Bool;
#endif
#endif
