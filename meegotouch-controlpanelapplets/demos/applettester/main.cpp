#include "main.h"
#include "window.h"

#include <stdio.h>
#include <QFile>
#include <MWidget>
#include <MApplication>
#include <QGraphicsLinearLayout>
#include <MLocale>

#include <dcpappletif.h>
#include <dcpwidget.h>
#include <dcpstylablewidget.h>

AppletTester::AppletTester (QObject *parent) :
    QObject (parent),
    mainWindow (0),
    loader (0),
    applet (0)
{

}

AppletTester::~AppletTester ()
{
    delete mainWindow;
    mainWindow = 0;

    delete applet;
    applet = 0;

    if (loader)
    {
        loader->unload ();
        delete loader;
        loader = 0;
    }
}

bool
AppletTester::loadAndRun (QString &plugin)
{
    if (! QFile::exists (plugin))
    {
        printf ("Error: %s is not exists!\n", qPrintable (plugin));
        return false;
    }

    if (! loader)
        loader = new QPluginLoader;

    loader->setFileName (plugin);
    loader->setLoadHints (QLibrary::ResolveAllSymbolsHint);

    if (! loader->load ())
    {
        printf ("Error: plugin can not be loaded: %s\nInfo: %s\n",
                qPrintable (plugin), qPrintable (loader->errorString ()));
        return false;
    }

    applet = qobject_cast<DcpAppletIf*>(loader->instance ());

    if (! applet)
    {
        printf ("Error: we're in a trouble, this plugin (%s) is "
                "not a controlpanel applet?!", qPrintable (plugin));
        return false;

    }

    /*
     * Init the applet
     */
    applet->init ();

    /*
     * Lets construct and show the window
     */
    mainWindow = new AppletTesterWindow;
    mainWindow->show ();

    /*
     * And lets show the plugins main page
     */
    showPluginWidget (0);

    return true;
}

void
AppletTester::showPluginWidget (int partId)
{
    if (! mainWindow)
    {
        printf ("ALERT: mainWindow is not yet created?!\n");
        return;
    }

    if (! applet)
    {
        printf ("ALERT: applet is not yet created or destroyed!\n");
        return;
    }

    MWidget *appletWidget = 0;

    appletWidget = applet->constructStylableWidget (partId);

    if (! appletWidget)
    {
        appletWidget = applet->constructWidget (partId);
    }

    if (! appletWidget)
    {
        printf ("Error: applet widget (partId = %d) is not available!\n", partId);
        return;
    }

    connect (appletWidget, SIGNAL (changeWidget (int)),
             this, SLOT (showPluginWidget (int)));

    MApplicationPage *page = mainWindow->generateAndShowPage ();

    QGraphicsLinearLayout *pageLayout = new QGraphicsLinearLayout (Qt::Vertical);
    pageLayout->setSpacing (0.);
    pageLayout->setContentsMargins (0., 0., 0., 0.);

    pageLayout->addItem (appletWidget);

    page->centralWidget ()->setLayout (pageLayout);
}

int main (int argc, char **argv)
{
    if (argc < 2)
    {
        printf ("Usage: %s path-to-dcp-plugin.so\n", argv[0]);
        return 1;
    }

    MApplication app (argc, argv);
    MLocale      locale;

    locale.installTrCatalog ("common");
    locale.installTrCatalog ("settings");
    locale.installTrCatalog ("wallpaper");
    locale.installTrCatalog ("energy");
    locale.installTrCatalog ("home");
    locale.installTrCatalog ("usb");
    locale.installTrCatalog ("display");
    locale.installTrCatalog ("language");
    locale.installTrCatalog ("profiles");
    locale.installTrCatalog ("sounds");
    locale.installTrCatalog ("volume");
    locale.installTrCatalog ("about-product");

    MLocale::setDefault (locale);

    AppletTester tester;

    int retval = -1;

    QString plugin (argv[1]);
    if (! tester.loadAndRun (plugin))
        return retval;

    retval = app.exec ();
    return retval;
}

