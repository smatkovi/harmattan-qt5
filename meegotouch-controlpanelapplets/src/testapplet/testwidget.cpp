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

#include "testwidget.h"

#include <QGraphicsLinearLayout>
#include <QStringList>
#include <MButton>
#include <MLabel>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MContainer>

#undef DEBUG
#include "../debug.h"

static const int MaxColumns = 2;

TestWidget::TestWidget (
        int                 numId,
        QGraphicsWidget    *parent) :
    DcpWidget (parent),
    m_NumId (numId)
{
    createWidgets ();
    retranslateUi ();
    createButtons ();
}

TestWidget::~TestWidget ()
{
}

void
TestWidget::createWidgets ()
{
    QGraphicsLinearLayout *mainLayout;
    MLabel                *tLabel;
    char                   title[512];

    snprintf (title, 512, "This is page %d at %p", m_NumId, this);

    tLabel = new MLabel (title);
    
    m_LocalContainer = createContainer ();

    mainLayout = new QGraphicsLinearLayout (Qt::Vertical);

    mainLayout->addItem (tLabel);
    mainLayout->addItem (m_LocalContainer);

    this->setLayout (mainLayout);
}

MContainer * 
TestWidget::createContainer ()
{
    MGridLayoutPolicy *policy;
    MLayout *layout = new MLayout();
    
    policy = new MGridLayoutPolicy (layout);
    layout->setPolicy (policy);
    
    MContainer *container = new MContainer ();
    container->centralWidget()->setLayout (layout);

    m_LocalLayoutPolicy = policy;

    return container;
}

void
TestWidget::retranslateUi ()
{
    if (m_LocalContainer) {
        m_LocalContainer->setTitle ("Activate page");
    }
}

void
TestWidget::createButtons ()
{
    int x, y;

    Q_ASSERT (m_LocalLayoutPolicy != 0);
    QStringList titleList;
    
    titleList <<
        "Page 0" <<
        "Page 1" <<
        "Page 2" <<
        "Page 3" <<
        "Page 4" <<
        "Page 5";


    int n = 0;
    foreach (QString title, titleList) {
        MButton *button;
        x = n / MaxColumns;
        y = n % MaxColumns;

        button = new MButton (title);
        
        SYS_DEBUG ("Adding page button to %d, %d", x, y);
        m_LocalLayoutPolicy->addItem (button, x, y);

        connect (button, SIGNAL(clicked()),
                this, SLOT(pagingClicked()));

        ++n;
    }

    MButton *backButton;
    backButton = new MButton ("Back");
    
    connect (backButton, SIGNAL(clicked()),
            this, SLOT(backClicked()));

    x = n / MaxColumns;
    y = n % MaxColumns;
    
    SYS_DEBUG ("Adding back button to %d, %d", x, y);
    m_LocalLayoutPolicy->addItem (backButton, x, y);
}


void 
TestWidget::pagingClicked ()
{
    MButton *button = qobject_cast<MButton*> (sender());
    QString label;
    int Id;

    if (button == 0) {
        SYS_WARNING ("This is not a button?");
    }

    label = button->text();
    SYS_DEBUG ("button '%s' clicked", SYS_STR(label));

    Id = -1;
    if (label == "Page 0")
        Id = 0;
    else if (label == "Page 1")
        Id = 1;
    else if (label == "Page 2")
        Id = 2;
    else if (label == "Page 3")
        Id = 3;
    else if (label == "Page 4")
        Id = 4;
    else if (label == "Page 5")
        Id = 5;

    if (Id == -1) {
        SYS_WARNING ("Label not found");
        return;
     }

    SYS_DEBUG ("Emitting changeWidget(%d)", Id);
    emit changeWidget (Id);
}

void 
TestWidget::backClicked ()
{
    SYS_DEBUG ("");
    closePage ();
}
