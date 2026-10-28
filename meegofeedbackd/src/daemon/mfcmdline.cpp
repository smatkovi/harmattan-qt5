/* This file is part of meegofeedbackd
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mfcmdline.h"

#include <iostream>

bool mfProcessCmdLineArguments(bool &daemonMode,
                               bool &log,
                               bool &syslog,
                               bool &verbose,
                               QStringList arguments)
{
    if (arguments.contains("--help")) {
        std::cout << "--daemon   Enables daemon mode\n"
                     "           Default: foreground mode\n";
        std::cout << "--log      Enable logging\n";
        std::cout << "--syslog   Redirect output to syslog\n";
        std::cout << "--verbose  Increased verbosity\n";
        return false;
    }

    if (arguments.contains("--daemon")) {
        daemonMode = true;
    }

    if (arguments.contains("--log")) {
        log = true;
    }

    if (arguments.contains("--syslog")) {
        syslog = true;
    }

    if (arguments.contains("--verbose")) {
        verbose = true;
    }

    return true;
}

