#ifndef _WINDOW_H
#define _WINDOW_H

#include <MApplicationWindow>
#include <MApplicationPage>

class AppletTesterWindow : public MApplicationWindow
{
Q_OBJECT

public:
    AppletTesterWindow (QWidget *parent = 0);
    ~AppletTesterWindow ();

    /*
     * This is going to be deleted after a dismiss
     */
    MApplicationPage *generateAndShowPage ();

private:
    MApplicationPage *firstPage;
};

#endif
