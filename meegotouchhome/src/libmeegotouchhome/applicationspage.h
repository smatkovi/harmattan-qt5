/***************************************************************************
**
** The Nokia N9's Applications view: every application in one grid, panned
** vertically, as the middle page of the home screen's three (the launcher
** of this release pans its pages horizontally, which the home's own pages
** do here). Written anew for the Qt 5 port: the N9's home (1.3.2) was not
** released.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef APPLICATIONSPAGE_H
#define APPLICATIONSPAGE_H

#include <MWidget>
#include <QMap>
#include <QSharedPointer>

class DeferredUpdate;
class LauncherButton;
class LauncherDataStore;
class MDesktopEntry;
class MFlowLayoutPolicy;
class MLayout;
class MPannableViewport;
class MStylableWidget;

class ApplicationsPage : public MWidget
{
    Q_OBJECT

public:
    /*!
     * \param dataStore the launcher's data store: which applications there are
     */
    ApplicationsPage(LauncherDataStore *dataStore, QGraphicsItem *parent = 0);
    virtual ~ApplicationsPage();

public Q_SLOTS:
    //! Pans to the first row
    void scrollToTop();

private Q_SLOTS:
    //! Makes the grid show the applications of the data store, by name
    void updateButtons();
    //! An application's desktop entry changed
    void updateButton(const QSharedPointer<MDesktopEntry> &entry);

private:
    //! Once for a burst of changes, while the home is to be seen
    DeferredUpdate *buttonsUpdate;
    LauncherDataStore *dataStore;
    MPannableViewport *viewport;
    // styled (ApplicationsGrid): the margins clear of the status bar
    MStylableWidget *grid;
    MLayout *layout;
    MFlowLayoutPolicy *policy;
    //! The buttons by their desktop entry's path
    QMap<QString, LauncherButton *> buttons;
};

#endif
