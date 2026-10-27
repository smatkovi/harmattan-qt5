/*
 * Qt 5 and Xlib in one file (included before everything else, see
 * swipe.pro): Qt's headers first, they refuse to come after Xlib's Status
 * macro and break on its None and Bool; then Xlib, its Bool a type.
 */
#ifndef MSWIPEGESTURE_PRELUDE_H
#define MSWIPEGESTURE_PRELUDE_H
#ifdef __cplusplus
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <QtOpenGL/QtOpenGL>
#include <QtDBus/QtDBus>
#include <X11/Xlib.h>
#undef Bool
typedef int Bool;
#endif
#endif
