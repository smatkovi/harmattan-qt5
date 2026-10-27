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
#ifndef UT_GCONFSTRINGCOMBO_H
#define UT_GCONFSTRINGCOMBO_H

#include <QtTest/QtTest>
#include <QObject>

class Ut_GConfStringComboTests : public QObject
{
Q_OBJECT

private slots:
/*
      void init();
      void cleanup();
      void initTestCase();
      void cleanupTestCase();
 */
      void gconfstringcomboConstructor_data ();
      void gconfstringcomboConstructor ();
      void gconfstringcomboChangedInGConf_data();
      void gconfstringcomboChangedInGConf();
      void gconfstringcomboCurrentIndexChanged_data();
      void gconfstringcomboCurrentIndexChanged();
      void gconfstringcomboRetranslateUi_data();
      void gconfstringcomboRetranslateUi();

private:
      void _setGConfReturnString (
              const QString   &key,
              const QString   &newValue);
};

#endif
