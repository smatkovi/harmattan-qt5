/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#include "soundsettings.h"

#include <QWidget>
#include <QDebug>
#include <QString>

#include <MLocale>
#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MImageWidget>
#include <MContentItem>
#include <MGConfItem>
#include <MButton>

#include <alerttonebrowser.h>
MyApplication::MyApplication (int &argc, char **argv) :
    MApplication (argc, argv),count(0)
{
    MButton *button;

    /*
     * Load the necessary translation for soundsettings widget
     */
    MLocale locale;
    locale.installTrCatalog ("sounds");
    
    /*
     *
     */
    mainwindow = new MApplicationWindow;
    page1 = new MApplicationPage;
    button = new MButton ("Start browser...");
    
    page1->setCentralWidget (button);
    page1->appear (mainwindow);
    
    mainwindow->show ();

    connect (button, SIGNAL(clicked()),
            this, SLOT(startBrowser()));
}

void
MyApplication::startBrowser ()
{
    AlertToneBrowser  *alertToneBrowser;
    QString file;
    
    if (count % 2)
        file = "Alablaster.aac";
    else
        file = "Alarm 2.aac";
    
    count++;

    qDebug()<<"File to be shown selected as"<<file<<endl;
    if (!page2) {
        /*
         * This demo program implements a selection page for custom alert tones.
         * The value will not be stored in the profile backend, it will handled
         * by the application in the signal handler.
         */
        alertTone = new AlertTone ("custom.alert.tone");
        alertTone->set (
                QVariant ("/usr/share/sounds/ring-tones/Nokia tune.aac"));

        connect (alertTone, SIGNAL(changed()),
                this, SLOT(valueChanged()));
        /*
         * The alerttonebrowser is fully automatic widget with a gallery item,
         * an ovi item and a list with the default sound file.
         */
        alertToneBrowser = new AlertToneBrowser (alertTone);
        connect (alertToneBrowser, SIGNAL(closePage()),
                this, SLOT(closeBrowser()));

        page2 = new MApplicationPage;
        page2->setCentralWidget (alertToneBrowser);
    }


    alertTone->set (QVariant(QString("/usr/share/sounds/ring-tones/") + file));

    #if 1
    // This one will keep the browser page in the memory while the program is
    // running.
    page2->appear(mainwindow, MSceneWindow::KeepWhenDone);
    #else
    // This one will destroy the browser page when it is dismissed by either the
    // done or the cancel button.
    page2->appear(mainwindow, MSceneWindow::DestroyWhenDismissed);
    #endif
}

void
MyApplication::closeBrowser ()
{
    qDebug("closeBrowser");
    if (page2)
        page2->dismiss ();
}

void
MyApplication::valueChanged ()
{
    qDebug("valueChanged");
    printf ("*** fileName  = '%s'\n", 
            qPrintable(alertTone->fileName()));
    printf ("*** niceName  = '%s'\n", 
            qPrintable(alertTone->niceName()));
    printf ("*** trackerId = '%s'\n", 
            qPrintable(alertTone->trackerId()));
    fflush (stdout);
}

/******************************************************************************
 * Other functions.
 */
int
main (int argc, char **argv)
{
    MyApplication    *app;

    app = new MyApplication (argc, argv);
    return app->exec();
}

