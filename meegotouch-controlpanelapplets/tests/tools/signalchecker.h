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
#ifndef SIGNALCHECKER_H
#define SIGNALCHECKER_H

#include <QObject>

class SignalCounter;

class SignalChecker {

    public:
        SignalChecker(QObject *obj);
        ~SignalChecker();
        void addSignalChecker(const char *signal);
        void check();
        void increaseSigCounter(const char *signal);
   private:
        QHash<QString, SignalCounter*> m_signalHasher;
        QObject *m_objToMonitor;
};
#endif
