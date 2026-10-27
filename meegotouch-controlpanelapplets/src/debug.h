/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

/******************************************************************************
 * The first part of the header can be loaded and loaded again, so we can turn
 * on and off the debug facility for each source file.
 */

/*
 * Unfortunately testrunner.py sometimes drops an exception because the debug
 * messages are not UTF-8 strings. This is most probably because I have some
 * filenames printed in the functional tests. Could not find one though, so I
 * disabled the messages here.
 */
#include <time.h>

//#ifdef UNIT_TEST
//#  undef DEBUG
//#endif

#ifndef UNIT_TEST
#define DEBUG_TO_FILE
#endif

#if defined(__ARMEL__) && !defined(DEBUG_TO_FILE)
//#if !defined(DEBUG_TO_FILE)
#  define DEBUG_TO_FILE
#  ifndef DEBUG_SUPPRESS_COLOR
#    define DEBUG_SUPPRESS_COLOR
#  endif
//# endif
#endif

/*
 * In production releases, we have to disable all the debug messages
 */
#ifdef PUBLIC_RELEASE
#   undef DEBUG
#   undef WARNING
#endif

/*
 * If the debug facility is enabled we also enable all the warning messages.
 */
#if defined(DEBUG) && !defined(WARNING)
#  define WARNING
#endif

#if defined(WARNING) && !defined(CRITICAL)
#  define CRITICAL
#endif
/*
 * The SYS_DEBUG macro is used to print normal debug messages.
 */
#undef SYS_DEBUG
#ifdef DEBUG
#  define SYS_DEBUG(...) SysDebug::sysPrintMsg (\
        QtDebugMsg, \
        __PRETTY_FUNCTION__, \
        __VA_ARGS__)
#else
#  define SYS_DEBUG(...) { /* Nothing... */ }
#endif

/*
 * The SYS_WARNING is used to print warning messages.
 */
#undef SYS_WARNING
#ifdef WARNING
#  define SYS_WARNING(...) SysDebug::sysPrintMsg (\
        QtWarningMsg, \
        __PRETTY_FUNCTION__, \
        __VA_ARGS__)
#else
#  define SYS_WARNING(...) { /* Nothing... */ }
#endif

/*
 * The SYS_CRITICAL is used to print warning messages.
 */
#undef SYS_CRITICAL
#ifdef CRITICAL
#  define SYS_CRITICAL(...) SysDebug::sysPrintMsg (\
        QtCriticalMsg, \
        __PRETTY_FUNCTION__, \
        __VA_ARGS__)
#else
#  define SYS_CRITICAL(...) { /* Nothing... */ }
#endif

#ifdef DEBUG_CLOCK_START
#  undef DEBUG_CLOCK_START
#endif
#ifdef DEBUG_CLOCK_END
#  undef DEBUG_CLOCK_END
#endif

#ifdef DEBUG 
#define DEBUG_CLOCK_START \
    clock_t debugTimerStart; \
    clock_t debugTimerEnd; \
    debugTimerStart = clock ();

#define DEBUG_CLOCK_END(text) \
    debugTimerEnd = clock (); \
    SYS_DEBUG ("++++++++++++++++++++++++++++++++"); \
    SYS_DEBUG ("Ellapsed time for %s: %g", \
            (text), \
            10000 * ((double) debugTimerEnd - debugTimerStart) / (double)CLOCKS_PER_SEC);
#else
#define DEBUG_CLOCK_START     {}
#define DEBUG_CLOCK_END(text) {}
#endif

/******************************************************************************
 * Here we have those parts that can be loaded only once, so we protect them
 * with DEBUG_H. 
 */
#ifndef DEBUG_H
#define DEBUG_H

#include <QTime>
#include <QtDebug>

#ifdef DEBUG_SUPPRESS_COLOR
#  define TERM_RED     ""
#  define TERM_YELLOW  ""
#  define TERM_GREEN   ""
#  define TERM_BLUE    ""
#  define TERM_NORMAL  ""
#  define TERM_BOLD    ""
#else 
#  define TERM_YELLOW  "\033[1;31m" 
#  define TERM_RED     "\033[1;33m" 
#  define TERM_GREEN   "\033[1;32m"
#  define TERM_BLUE    "\033[1;34m"
#  define TERM_NORMAL  "\033[0;39m"
#  define TERM_BOLD    "\033[1m"
#endif

namespace SysDebug
{
    void sysPrintMsg (
        QtMsgType     type,
        const char   *function,
        const char   *formatstring,
        ...);
    void closeDebugFile ();
};

#define SYS_STR(qstring) (TERM_BOLD+qstring+TERM_NORMAL).toLatin1().constData()
#define SYS_BOOL(boolean) (boolean ? \
        TERM_BOLD "true" TERM_NORMAL : \
        TERM_BOLD "false" TERM_NORMAL)

#define SYS_TIME_STR SYS_STR(QTime::currentTime ().toString ("hh:mm:ss.zzz"))

#define IMPLEMENT_EGGS
#define EGGS_GCONF_KEY "/apps/mcpeggs"

#endif
