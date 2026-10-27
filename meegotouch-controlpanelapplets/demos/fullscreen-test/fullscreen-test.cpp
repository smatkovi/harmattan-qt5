/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#include <QWidget>
#include <QDebug>
#include <QString>

#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MImageWidget>
#include <MContentItem>
#include <MGConfItem>
#include <MButton>
#include <MLabel>

#include "fullscreen-test.h"

MyApplication::MyApplication (int &argc, char **argv) :
    MApplication (argc, argv)
{
    MButton *button;
    
    /*
     * Creating a main window and a page that shows one button.
     */
    mainwindow = new MApplicationWindow;
    page1 = new MApplicationPage;
    button = new MButton ("Add a page...");
    
    page1->setCentralWidget (button);
    page1->appear (mainwindow);
    
    mainwindow->show ();

    connect (button, SIGNAL(clicked()),
            this, SLOT(startSecondPage()));
}

void
MyApplication::startSecondPage ()
{
    MLabel *label;

    /*
     * Initializing.
     */
    page2 = new MApplicationPage;
    label = new MLabel ("Second page");
    
    page2->setCentralWidget (label);

    page2->appear(mainwindow);
    mainwindow->showFullScreen ();
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

