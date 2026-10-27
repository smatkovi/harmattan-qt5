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

#include <MApplication>
#include <MApplicationWindow>
#include <QString>
#include <QStringList>
#include <MListFilter>

#include  "ut_alerttonedefaults.h"
#include  "alerttonedefaultsmodel.h"

/******************************************************************************
 * Ut_AlertToneDefaults implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_alerttonedefaults",
      NULL };

void
Ut_AlertToneDefaultsTests::initTestCase()
{
      m_App = new MApplication(argc, argv);
}


void
Ut_AlertToneDefaultsTests::cleanupTestCase()
{
      if (m_App)
      delete m_App;
}

void
Ut_AlertToneDefaultsTests::alerttonedefaultsConstructor ()
{
      MApplicationWindow aw;

      AlertTone at("email.alert.tone");
      AlertToneDefaults atd(&at);

      QCOMPARE(atd.objectName () ,QString("AlertToneDefaults") );
      QCOMPARE(atd.m_tone->key() ,QString("email.alert.tone") );

}

void
Ut_AlertToneDefaultsTests::alerttonedefaultsItemClicked()
{
      MApplicationWindow aw;

      AlertTone *at = new AlertTone("email.alert.tone");
      testATD = new AlertToneDefaults(at);

      /*
       * We need to give a chance for the model to load the items.
       */
      int n = 0;
      while (!testATD->m_DefaultsModel->isFinished()) {
        QTest::qWait(200);
        
        QVERIFY (n < 130);
        ++n;
      }
      
      connect(testATD, SIGNAL(defaultItemClicked(const QString &)), 
            this, SLOT(receiveItemClicked(const QString &)));
            
      QStandardItemModel *model = (QStandardItemModel *) testATD->filtering()->proxy();
      if ( model->rowCount() > 1)
      {
			QModelIndex firstItem  = model->indexFromItem ( model->item(0) );
            testATD->itemClicked(firstItem);
       }
}

void 
Ut_AlertToneDefaultsTests::receiveItemClicked(const QString & itemName)
{
	  AlertToneDefaultsModel *model = testATD->m_DefaultsModel;			            	
      QCOMPARE(itemName,model->fileName(model->index(0, 0)));
      delete testATD;
}

void
Ut_AlertToneDefaultsTests::alerttonedefaultsToneChanged()
{
      MApplicationWindow aw;

      AlertTone * at = new AlertTone("email.alert.tone");
      AlertToneDefaults atd(at);
      QStandardItemModel *model = (QStandardItemModel *) atd.itemModel ();

      	for (int Nix = 0 ; Nix < model ->rowCount() ; Nix++)
      	{
      	      QVariant  tone;
                  tone = model->data(model->index(Nix, model->columnCount() - 1));
                  if (atd.m_tone->value() != tone)
                  {
                        atd.m_tone->m_val = tone;
                        atd.toneChanged();
                      	for (int i = 0 ; i < model ->rowCount() ; i++)
                        {
                              if (   atd.selectionModel()->isSelected ( model->index(i, 0)  ))
                              {
                                    QCOMPARE(model->data(model->index(i, model->columnCount() - 1)),tone);
                                    break;
                              }
                        }
                        break;
                  }
      	}
}

#if 0
/*
 * FIXME: AlertToneDefaults::selectedIndex () has been removed.
 */
void 
Ut_AlertToneDefaultsTests::alerttonedefaultsSelectedIndex ()
{
      AlertTone * at = new AlertTone("email.alert.tone");
      AlertToneDefaults atd(at); 
      atd.selectionModel()->clear();
      atd.selectionModel()->select(atd.defaultsModel->index(atd.defaultsModel->rowCount()-1, 0), QItemSelectionModel::Select);
      QCOMPARE(atd.selectedIndex(),atd.defaultsModel->rowCount()-1);
}
#endif

#if 0
/*
 * FIXME: AlertToneDefaults::selectedFile() has been removed.
 */
void 
Ut_AlertToneDefaultsTests::alerttonedefaultsSelectedFile ()
{
      AlertTone * at = new AlertTone("email.alert.tone");
      AlertToneDefaults atd(at);
      atd.selectionModel()->clear();
      atd.selectionModel()->select(atd.defaultsModel->index(atd.defaultsModel->rowCount()-1, 0), QItemSelectionModel::Select);
      QCOMPARE(atd.selectedFile(),atd.defaultsModel->data(atd.defaultsModel->index(atd.defaultsModel->rowCount()-1, 0)).toString()); 
}
#endif

#if 0
/*
 * FIXME: AlertToneDefaults::rowCount() has been removed.
 */
void
Ut_AlertToneDefaultsTests::alerttonedefaultsRowCount()
{
      AlertTone * at = new AlertTone("email.alert.tone");
      AlertToneDefaults atd(at);    
      QCOMPARE( atd.rowCount(),atd.defaultsModel->rowCount());
}
#endif
QTEST_APPLESS_MAIN(Ut_AlertToneDefaultsTests)
