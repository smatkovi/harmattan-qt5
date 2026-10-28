/***************************************************************************
**
** The Nokia N9's Open applications view: the switcher's windows in a grid
** that pans vertically, as the third page of the home screen's three (the
** switcher view of this release pans its pages horizontally, which the
** home's own pages do here). Chosen with the view type "vertical". Written
** anew for the Qt 5 port: the N9's home (1.3.2) was not released.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef VERTICALSWITCHERVIEW_H
#define VERTICALSWITCHERVIEW_H

#include "switcherviewbase.h"
#include "switcherstyle.h"

class MGridLayoutPolicy;

class VerticalSwitcherView : public SwitcherViewBase
{
    Q_OBJECT
    M_VIEW(SwitcherModel, SwitcherStyle)

public:
    VerticalSwitcherView(Switcher *controller);
    virtual ~VerticalSwitcherView();

protected:
    //! \reimp
    virtual void setupModel();
    virtual void updateData(const QList<const char *> &modifications);
    virtual void applyStyle();
    // its own style (the N9's margins and columns), not the overview's
    virtual void applySwitcherMode();
    // the N9's view has one grid; no detail view to pinch to
    virtual void pinchGestureEvent(QGestureEvent *event, QPinchGesture *gesture);
    //! \reimp_end

private Q_SLOTS:
    //! Puts the model's buttons into the grid
    void updateButtons();

private:
    MGridLayoutPolicy *policy;
};

#endif
