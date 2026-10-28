/*
 * Where the keyboard's layouts are: /usr/share/meegotouch/virtual-keyboard/
 * layouts/, or M_VKB_LAYOUTS_DIR for a staged test (as M_THEME_DIR is for
 * the themes).
 *
 * This file is part of meego-keyboard (BSD licence, see LICENSE).
 */
#ifndef LAYOUTSPATH_H
#define LAYOUTSPATH_H

#include <QString>
#include <QtGlobal>

inline QString vkbLayoutsPath()
{
    const QByteArray dir = qgetenv("M_VKB_LAYOUTS_DIR");
    if (dir.isEmpty())
        return QString("/usr/share/meegotouch/virtual-keyboard/layouts/");
    return QString::fromLocal8Bit(dir) + (dir.endsWith('/') ? "" : "/");
}

#endif
