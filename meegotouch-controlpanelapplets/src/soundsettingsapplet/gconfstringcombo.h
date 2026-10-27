/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#ifndef _GCONF_STRING_COMBO_H_
#define _GCONF_STRING_COMBO_H_

#include <MComboBox>
class MGConfItem;

/*!
 * A simple combobox widget that holds a list of possible GConf string values,
 * shows them as translated UI strings and does the synchronization between the
 * UI and the GConf database in both ways.
 */
class GConfStringCombo : public MComboBox
{
Q_OBJECT

public:
    typedef enum {
        ColumnUiString = 0,
        ColumnGConfString,
    } GConfStringComboColumns;

    GConfStringCombo (const QString &gconfKey,
                      const QStringList &possibleValues,
                      QGraphicsItem *parent = NULL);
    ~GConfStringCombo ();

public slots:
    void retranslateUi ();

private slots:
    void changedInGConf ();
    void currentIndexChanged (int index);

private:
    MGConfItem      *m_gconfItem;
    QStringList      m_possibleValues;
#ifdef UNIT_TEST
    friend class Ut_GConfStringComboTests;
#endif
};

#endif
