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
#include "ut_alerttonepreview.h"
#include "alerttonepreview.h"

#include <stdlib.h>
#include <string.h>
#include <MApplication>
#include <MApplicationWindow>
#include <QString>
#include <QStringList>

#include "qprofilevalue.h"

/******************************************************************************
 * Stubs
 */

#ifdef HAVE_LIBRESOURCEQT
#include <policy/resource-set.h>
namespace ResourcePolicy {
//ResourceSet::ResourceSet(const QString &applicationClass, QObject *parent)
//{
//    Q_UNUSED(applicationClass);
//    Q_UNUSED(parent);
//}

bool
ResourceSet::addResource (ResourceType resourceType)
{
    Q_UNUSED(resourceType);
    return true;
}

void
ResourceSet::addResourceObject (Resource *resource)
{
    Q_UNUSED(resource);
}

bool
ResourceSet::acquire ()
{
    return true;
}

bool
ResourceSet::release ()
{
    return true;
}

//AudioResource::AudioResource (const QString &audioGroup)
//{
//    Q_UNUSED(audioGroup);
//}

void
AudioResource::setProcessID (quint32 newPID)
{
    Q_UNUSED(newPID);
}

void
AudioResource::setStreamTag (const QString &name, const QString &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

}

#endif


/******************************************************************************
 * Ut_AlertTonePreview implementation.
 */

void
Ut_AlertTonePreviewTests::initTestCase()
{
      char *argv1;

      m_argc = 1;
      m_argv = (char **)malloc(2 * sizeof(char *));
      memset(m_argv, 0, 2 * sizeof(char *));
      argv1 = (char *)malloc(strlen("my_argv") + 1);
      strcpy(argv1, "my_argv");
      (*m_argv) = argv1;

      gst_init(&m_argc, &m_argv);

      m_App = new MApplication(m_argc, m_argv);
}

void
Ut_AlertTonePreviewTests::cleanupTestCase()
{
	gst_deinit();

	free((*m_argv));
	free(m_argv);
}

void
Ut_AlertTonePreviewTests::alerttonepreviewConstructor ()
{
      MApplicationWindow aw;
      AlertTonePreview  atp("/usr/share/sounds/ring-tones/Lucid dreaming.aac");
      atp.gstInit();

      QVERIFY(atp.m_gstPipeline != 0 );
      QVERIFY(atp.m_gstFilesrc != 0 );
      QVERIFY(atp.m_gstVolume != 0 );
      QCOMPARE(atp.m_profileVolume.key(), QString( "ringing.alert.volume@general" ));
}

void
Ut_AlertTonePreviewTests::alerttonepreviewfname()
{
      MApplicationWindow aw;
      AlertTonePreview  atp("/usr/share/sounds/ring-tones/Lucid dreaming.aac");
      atp.gstInit();
      QCOMPARE(atp.fname(), QString( "/usr/share/sounds/ring-tones/Lucid dreaming.aac" ));
}

void
Ut_AlertTonePreviewTests::alerttonepreviewProfileToGstVolume()
{
      MApplicationWindow aw;
      AlertTonePreview  atp("/usr/share/sounds/ring-tones/Lucid dreaming.aac");
      atp.gstInit();
      qDebug() << atp.profileToGstVolume();
      double  volume;
      volume = atp.profileToGstVolume();
      QVERIFY ( volume >= 0 && volume <= 1 );
}

void
Ut_AlertTonePreviewTests::alerttonepreviewProfileVolumeChanged()
{
      MApplicationWindow aw;
      AlertTonePreview  atp("/usr/share/sounds/ring-tones/Lucid dreaming.aac");
      atp.gstInit();
      double  *  cnt = new double;
      atp.m_profileVolume.set(QVariant(50));
      atp.m_profileVolume.emit_changed();

      g_object_get(G_OBJECT(atp.m_gstVolume), "volume", cnt, NULL);

      QVERIFY (qFuzzyCompare ( (float) *cnt, (float) 0.5 ) );

      atp.m_profileVolume.set(QVariant(90));
      atp.m_profileVolume.emit_changed();

      g_object_get(G_OBJECT(atp.m_gstVolume), "volume", cnt, NULL);
      QVERIFY (qFuzzyCompare ( (float) *cnt, (float) 0.9 ) );
}

QTEST_APPLESS_MAIN(Ut_AlertTonePreviewTests)
