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

#ifndef _PROFILE_INT_COMBO_H_
#define _PROFILE_INT_COMBO_H_

#include <MComboBox>
#include <QString>
#include "qprofilevalue.h"

class QGraphicsItem;

class ProfileIntCombo : public MComboBox
{
	Q_OBJECT

public:
    typedef enum {
        ColumnUiString = 0,
        ColumnProfileInt,
    } GConfIntComboColumns;

	ProfileIntCombo(const QString &key, bool setAllProfiles = false, QGraphicsItem *parent = 0);

public slots:
	void retranslateUi();

private slots:
	void changedInProfile();
	void currentIndexChanged(int index);

private:
	QProfileValue m_val;
#ifdef UNIT_TEST
    friend class Ut_ProfileintComboTests;
#endif /* !UNIT_TEST */
};

#endif /* !_PROFILE_INT_COMBO_H_ */
