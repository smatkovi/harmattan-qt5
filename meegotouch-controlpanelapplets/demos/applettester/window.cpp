#include "window.h"

#include <MAction>
#include <MApplication>

AppletTesterWindow::AppletTesterWindow (QWidget *parent) :
    MApplicationWindow (parent),
    firstPage (0)
{
    setObjectName ("AppletTesterWindow");
    setStyleName ("CommonApplicationWindowInverted");

    setPortraitOrientation ();
    lockOrientation ();
}

AppletTesterWindow::~AppletTesterWindow ()
{
    firstPage = 0;
}

MApplicationPage *
AppletTesterWindow::generateAndShowPage ()
{
    MApplicationPage *page = new MApplicationPage;
    page->setStyleName ("CommonApplicationPageInverted");

    // Always show the navigation bar
    page->setComponentsDisplayMode (MApplicationPage::NavigationBar |
                                    MApplicationPage::EscapeButton,
                                    MApplicationPageModel::Show);

    MAction *action = new MAction ("icon-m-common-blue", "text", page);
    action->setLocation (MAction::ToolBarLocation);
    page->addAction (action);

    connect (action, SIGNAL (triggered (bool)),
             page, SLOT (dismiss ()));

    page->setPannable (true);

    if (! firstPage)
    {
        firstPage = page;

        // This will cause automatically dismissal and window closing
        page->setEscapeMode (MApplicationPageModel::EscapeCloseWindow);

        connect (page, SIGNAL (disappeared ()),
                 MApplication::instance (), SLOT (quit ()));
    }

    // show it and destroy once it is closed
    page->appear (this, MSceneWindow::DestroyWhenDone);

    return page;
}

