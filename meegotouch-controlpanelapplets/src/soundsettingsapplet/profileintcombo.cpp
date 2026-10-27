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

#include <QDebug>
#include "profileintcombo.h"
#include "alerttoneappletmaps.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

ProfileIntCombo::ProfileIntCombo(
        const QString    &key, 
        bool              setAllProfiles, 
        QGraphicsItem    *parent):
	MComboBox(parent),
	m_val(key, setAllProfiles)
{
	QAbstractItemModel *model = itemModel();
	model->insertColumn(ColumnProfileInt);

	connect (this, SIGNAL(currentIndexChanged(int)), 
            this, SLOT(currentIndexChanged(int)));
	connect (&m_val, SIGNAL(changed()), 
            this, SLOT(changedInProfile()));

	retranslateUi();
}

void
ProfileIntCombo::retranslateUi()
{
    SYS_DEBUG ("");	
	QAbstractItemModel *model = itemModel();
	QProfileValue::RangeType rangeType;
	QList<QVariant> possibleValues = m_val.possibleValues(&rangeType);
    int                 from, to;
    /*
     * The title of combobox.
     */
	setProperty("title", AlertToneAppletMaps::mapToUiString(m_val.key()));

    /*
     * Removing all rows, then adding the translated items.
     */
	model->removeRows(0, model->rowCount());

	if (QProfileValue::Interval == rangeType) {
        from = possibleValues[0].toInt();
        to   = possibleValues[1].toInt();

		for (int Nix = from; Nix <= to; Nix++) {
			model->insertRow(model->rowCount());
			model->setData (
                    model->index(Nix, ColumnUiString),
                    AlertToneAppletMaps::mapToUiString(Nix));
			model->setData (
                    model->index(Nix, ColumnProfileInt), 
                    QVariant(Nix));
		}
    } else {
		for (int Nix = 0 ; Nix < possibleValues.size() ; Nix++) {
			model->insertRow(model->rowCount());
			model->setData (
                    model->index(Nix, ColumnUiString), 
                    AlertToneAppletMaps::mapToUiString(possibleValues[Nix].toInt()));
			model->setData (
                    model->index(Nix, ColumnProfileInt), 
                    QVariant(possibleValues[Nix]));
		}
    }

	changedInProfile();
}

void
ProfileIntCombo::changedInProfile()
{
	QAbstractItemModel *model = itemModel();
	int val = m_val.value().toInt();

    SYS_DEBUG ("*** currentIndex() = %d", currentIndex());
    SYS_DEBUG ("*** val         () = %d", val);

    /*
     * If the item is already shown as current item we don't need to do
     * anything.
     */
    if (currentIndex() >= 0 &&
        model->data(model->index(currentIndex(), ColumnProfileInt)).toInt() ==
        val)
        return;
    
    /*
     * Let's see where is that item and let's select it.
     */
	for (int Nix = 0 ; Nix < count() ; Nix++) {
        if (model->data(model->index(Nix, ColumnProfileInt)).toInt() == val) {
            setCurrentIndex(Nix);
            break;
        }
    }
}

/*
 * This slot is called when the user selected a new item from the combobox. The
 * method will write the new selection to the profile.
 */
void
ProfileIntCombo::currentIndexChanged(int index)
{
    SYS_DEBUG ("");

	QAbstractItemModel *model = itemModel();
	m_val.set(model->data(model->index(index, ColumnProfileInt)));
}
