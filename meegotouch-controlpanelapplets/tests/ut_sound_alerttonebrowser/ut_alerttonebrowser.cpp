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
#include <MWindow>
#include <QString>
#include <QStringList>
#include <QSignalSpy>
#include <QVariant>
#include <QList>
#include "ut_alerttonebrowser.h"
#include "alerttonebrowser.h"
#include "drilldownitem.h"

#include <MContentItem>
#include <MSceneWindow>
#include <QGraphicsLinearLayout>

#define DEBUG
#define WARNING
#include "../debug.h"

static const QString validSoundFile1 = "/usr/share/sounds/ring-tones/Beep.aac";

#ifdef HAVE_LIBRESOURCEQT
#include <policy/resource-set.h>
#endif

/******************************************************************************
 * Stubs
 */

#ifdef HAVE_LIBRESOURCEQT
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
    return false;
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

bool stubAppearCalled = false;
void
MSceneWindow::appear (MWindow *window, MSceneWindow::DeletionPolicy policy)
{
    SYS_DEBUG ("");
    Q_UNUSED (window);
    Q_UNUSED (policy);

    stubAppearCalled = true;
}


/******************************************************************************
 * Ut_AlertToneBrowser implementation.
 */
class MCustomContentItem: public MContentItem
{
public:
    MCustomContentItem (MContentItem::ContentItemStyle itemStyle=MContentItem::IconAndTwoTextLabels, QGraphicsItem *parent=0)
    :MContentItem(itemStyle,parent),fullPath("") {}

    QString fullPath;
};

void
Ut_AlertToneBrowserTests::init()
{
}

void
Ut_AlertToneBrowserTests::cleanup()
{
}

static int   m_argc = 1;
static char *m_argv[] = { (char*) "./ut_alerttonebrowser", NULL };

void
Ut_AlertToneBrowserTests::initTestCase()
{
    m_App = new MApplication (m_argc, m_argv);

    at = new AlertTone ("email.alert.tone");
    savedAlertTone = at->fileName ();

    m_window = new MWindow;
    m_window->show ();
}

void
Ut_AlertToneBrowserTests::cleanupTestCase()
{
    if (at)
    {
        at->set(QVariant (savedAlertTone));
        delete at;
        at = 0;
    }

    delete m_window;
    m_window = 0;

    if (m_App)
        delete m_App;
}

void
Ut_AlertToneBrowserTests::alerttonebrowserConstructor ()
{
    AlertTone at("email.alert.tone");
    AlertToneBrowser atbt(&at);

    QCOMPARE (atbt.m_tone->key(), QString ("email.alert.tone"));
#ifdef HAVE_CONTENT_MANAGER
    QCOMPARE (atbt.m_my_music->objectName (),
              QString ("MContentItem_pickFromMyMusic"));
#endif
    if (atbt.m_ovi_store)
        QCOMPARE (atbt.m_ovi_store->objectName (),
                  QString ("MContentItem_getMoreFromOviStore"));

    // this will fails to find the MApplicationPage
    atbt.polishEvent ();
    // this does nothing basically, just sets the focus on the AlertToneDefaults
    atbt.defaultsDisplayEntered ();
}

void
Ut_AlertToneBrowserTests::alerttonebrowserRetranslateUi()
{
    AlertToneBrowser atbt(at);
    atbt.retranslateUi();

#ifdef HAVE_CONTENT_MANAGER
    QCOMPARE (atbt.m_my_music->property ("title").toString (),
              qtTrId ("qtn_sond_pick_music"));
#endif
    if (atbt.m_ovi_store)
        QCOMPARE (atbt.m_ovi_store->property ("title").toString(),
                  qtTrId ("qtn_sond_store"));
}

void
Ut_AlertToneBrowserTests::alerttonebrowserSetAlertTone()
{
    const QString filename = "/usr/share/sounds/ring-tones/Polaris.aac";
    AlertToneBrowser atbt(at);

    /*
     * Setting the alert tone and checking if the value has been set. FIXME: is
     * this properly stubbed?
     * Please note that setAlertTone() is not starting the playback any more.
     */
    atbt.setAlertTone(filename);
    QCOMPARE(atbt.currSelectedFile, filename);

    /*
     * Checking the playback.
     */
    atbt.stopPlayingSound();
    atbt.startPlayingSound (filename);
    QVERIFY(atbt.m_preview != NULL);

    // Setting the same filename again should stop the playback!
    atbt.startPlayingSound (filename);
    QVERIFY(atbt.m_preview == NULL);

    atbt.set (filename);
    QCOMPARE (atbt.currSelectedFile, filename);
}

/*!
 * FIXME: This test depends on the filesystem, the file must exists.
 */
void
Ut_AlertToneBrowserTests::alerttonebrowserAccept()
{
    AlertToneBrowser atbt(at);

    atbt.currSelectedFile = validSoundFile1;
    atbt.accept();

    QCOMPARE(at->fileName(), validSoundFile1);
}

void
Ut_AlertToneBrowserTests::alerttonebrowserCancel ()
{
    AlertToneBrowser atbt (at);
    QSignalSpy       spy (&atbt, SIGNAL (closePage ()));

    atbt.cancel ();

    QCOMPARE (spy.count (), 1);
}

void
Ut_AlertToneBrowserTests::testLaunchMusicBrowser ()
{
    stubAppearCalled = false;

#ifdef HAVE_CONTENT_MANAGER
    AlertToneBrowser atbt (at);

    SYS_DEBUG ("*** HAVE_CONTENT_MANAGER");

    atbt.launchMusicBrowser ();
#ifndef USE_CONTENT_ITEM_SHEET
    QVERIFY (atbt.m_MusicBrowser);
#endif

    QCOMPARE (stubAppearCalled, true);

#ifdef USE_CONTENT_ITEM_SHEET
    // this does nothing as we can not select music from unit-test
    atbt.contentItemsSheetDoneClicked ();
#endif

    //Now lets close it
    atbt.browserBackButtonClicked ();

#ifndef USE_CONTENT_ITEM_SHEET
    QVERIFY (! atbt.m_MusicBrowser);
#endif

#endif
}

QTEST_APPLESS_MAIN(Ut_AlertToneBrowserTests)
