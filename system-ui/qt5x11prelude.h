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
#include <X11/Xlib.h>
#undef Bool
typedef int Bool;
#endif
#endif
