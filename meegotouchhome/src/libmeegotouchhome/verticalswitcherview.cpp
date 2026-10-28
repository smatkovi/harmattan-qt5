/***************************************************************************
**
** The Nokia N9's Open applications view (see verticalswitcherview.h).
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "verticalswitcherview.h"
#include "switcher.h"
#include "switcherbutton.h"
#include "mainwindow.h"

#include <MGridLayoutPolicy>
#include <MLayout>
#include <MPannableViewport>
#include <MSceneManager>
#include <MViewCreator>
#include <QGestureEvent>
#include <QGraphicsLinearLayout>
#include <QPinchGesture>

VerticalSwitcherView::VerticalSwitcherView(Switcher *switcher) :
    SwitcherViewBase(switcher),
    policy(0)
{
    viewport = new MPannableViewport;
    viewport->setObjectName("VerticalSwitcherViewport");
    // vertical only: a sideways pan belongs to the home's pages
    viewport->setPanDirection(Qt::Vertical);
    mainLayout->addItem(viewport);
    viewport->setWidget(pannedWidget);

    policy = new MGridLayoutPolicy(pannedLayout);
    policy->setObjectName("OverviewPolicy");
    pannedLayout->setPolicy(policy);

    connect(MainWindow::instance()->sceneManager(), SIGNAL(orientationChanged(M::Orientation)),
            SLOT(updateButtons()));
}

VerticalSwitcherView::~VerticalSwitcherView()
{
}

void VerticalSwitcherView::setupModel()
{
    SwitcherViewBase::setupModel();
    updateButtons();
}

void VerticalSwitcherView::updateData(const QList<const char *> &modifications)
{
    SwitcherViewBase::updateData(modifications);
    foreach (const char *member, modifications) {
        if (member == SwitcherModel::Buttons)
            updateButtons();
    }
}

void VerticalSwitcherView::applyStyle()
{
    SwitcherViewBase::applyStyle();
    updateButtons();
}

void VerticalSwitcherView::applySwitcherMode()
{
    controller->setStyleName("VerticalSwitcher");
}

void VerticalSwitcherView::pinchGestureEvent(QGestureEvent *event, QPinchGesture *gesture)
{
    event->ignore(gesture);
}

void VerticalSwitcherView::updateButtons()
{
    if (!policy)
        return;
    removeButtonsFromLayout();

    // A fresh grid keeps the panned widget's size right (as SwitcherView)
    MGridLayoutPolicy *old = policy;
    policy = new MGridLayoutPolicy(pannedLayout);
    policy->setObjectName("OverviewPolicy");
    policy->setHorizontalSpacing(style()->buttonHorizontalSpacing());
    policy->setVerticalSpacing(style()->buttonVerticalSpacing());
    pannedLayout->setPolicy(policy);
    delete old;

    // the switcher style's columns (two, three with more windows, as the N9)
    const int count = model()->buttons().count();
    int columns = qMax(1, style()->columnsPerPage());
    if (count > columns * qMax(1, style()->rowsPerPage()))
        ++columns;
    const SwitcherButtonModel::ViewModeType mode = columns > 2 ? SwitcherButtonModel::Small
                                                               : SwitcherButtonModel::Medium;
    int i = 0;
    foreach (QSharedPointer<SwitcherButton> button, model()->buttons()) {
        button->setObjectName("OverviewButton");
        button->model()->setViewMode(mode);
        policy->addItem(button.data(), i / columns, i % columns);
        ++i;
    }
    viewport->setVerticalPanningPolicy(count > 0 ? MPannableWidget::PanningAsNeeded
                                                 : MPannableWidget::PanningAlwaysOff);
}

M_REGISTER_VIEW_NEW(VerticalSwitcherView, Switcher)
