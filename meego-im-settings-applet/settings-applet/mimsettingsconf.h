/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMSETTINGSCONF_H
#define MIMSETTINGSCONF_H

#include <QObject>
#include <QList>

class MInputMethodPlugin;
class MAbstractInputMethodSettings;
class QDBusInterface;

class MImSettingsConf : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MImSettingsConf)
public:
    ~MImSettingsConf();

    /*!
     * \brief Get singleton instance
     * \return singleton instance
     */
    static MImSettingsConf &instance();

    //! \brief Create singleton
    static void createInstance();

    //! \brief Destroy singleton
    static void destroyInstance();

    struct MImSubView {
        QString pluginName;
        QString subViewId;
        QString subViewTitle;
    };

    /*!
     * \brief Return all available subViews.
     */
    QList<MImSubView> subViews() const;

private:
    MImSettingsConf();
    void connectToIMPluginManagerDBus();

    //! Singleton instance
    static MImSettingsConf *imSettingsConfInstance;

    QDBusInterface *impluginMgrIface;

    friend class Ut_MIMSettingsConf;
    friend class Ft_MIMSettingsConf;
};

inline MImSettingsConf &MImSettingsConf::instance()
{
    Q_ASSERT(imSettingsConfInstance);
    return *imSettingsConfInstance;
}

#endif
