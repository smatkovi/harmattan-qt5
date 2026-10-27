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
#include "alerttonebrowser.h"
#include "alerttonepreview.h"
#include "alerttonedefaults.h"
#include "alerttoneappletmaps.h"
#include "trackerconnection.h"
#include "drilldownitem.h"
#include "soundsettingsutils.h"

#include <QGraphicsLinearLayout>
#include <QDBusInterface>
#include <MLabel>

#ifdef HAVE_CONTENT_MANAGER
#include <ContentItemsPage.h>
#  ifdef USE_CONTENT_ITEM_SHEET
#    include <ContentItemsSheet.h>
#  endif
#endif

#include <MList>
#include <MListFilter>
#include <MApplication>
#include <MApplicationPage>
#include <MApplicationWindow>
#include <MWindow>
#include <MAction>
#include <MTextEdit>
#include <MImageWidget>
#include <MContentItem>
#include <MSeparator>
#include <QDBusPendingCall>

#include "../styles.h"


#define DEBUG
#define WARNING
#include "../debug.h"

static const int filterEditorPosition = 2;

/*!
 * An MContentItem sligthly modified
 */
class MCustomContentItem: public MContentItem
{
public:
    /*!
     * An overloaded constructor for this custom content-item
     */
    MCustomContentItem (MContentItem::ContentItemStyle itemStyle =
                        MContentItem::IconAndTwoTextLabels,
                        QGraphicsItem *parent=0)
    : MContentItem (itemStyle, parent), fullPath ("") {}

    /*!
     * For storing the ringtone full-path
     */
    QString fullPath;
};

/******************************************************************************
 * AlertToneBrowser implementation.
 */

/*!
 * Constructor for the AlertToneBrowser widget
 *
 * \param tone An AlertTone instance
 * \param parent The parent widget
 */
AlertToneBrowser::AlertToneBrowser(AlertTone *tone, QGraphicsWidget *parent):
    AlertToneToplevel (parent),
    m_tone (tone),
    m_ovi_store (0),
    m_defaults (0),
    m_preview (0),
    #ifndef USE_CONTENT_ITEM_SHEET
    m_MusicBrowser (0),
    #endif
    m_DoneAction (0),
    m_CancelAction (0)
{
    SYS_DEBUG ("");
    /*
     * FIXME: Why do we need to set the title?
     */
    setProperty ("title", AlertToneAppletMaps::mapToUiString (m_tone->key ()));

    createContent();
}

/*!
 * Destructor for the AlertToneBrowser widget
 */
AlertToneBrowser::~AlertToneBrowser()
{
    stopPlayingSound ();

    if (m_defaults)
    {
        delete m_defaults;
        m_defaults = 0;
    }
}

void
AlertToneBrowser::createContent()
{
    bool        needOvi = m_tone->key().startsWith ("ringing");
    MSeparator *spacer;

    SYS_DEBUG ("****************************************");
    SYS_DEBUG ("*** key = %s", SYS_STR(m_tone->key()));
    m_MainLayout = new QGraphicsLinearLayout (Qt::Vertical);
    m_MainLayout->setContentsMargins (0., 0., 0., 0.);
    m_MainLayout->setSpacing (0.);
    setLayout (m_MainLayout);

    /*
     *
     */
    m_TitleLabel = new MLabel;
    m_TitleLabel->setStyleName (APP_TITLE_LABEL_STYLE_NAME);
    m_MainLayout->addItem (m_TitleLabel);

#if  APP_TITLE_USES_SPACER
    spacer = new MSeparator;
    spacer->setStyleName ("CommonItemDividerInverted");
    m_MainLayout->addItem (spacer);
#endif

#ifdef HAVE_CONTENT_MANAGER
    // "Pick from My Music"
    m_my_music = new RightArrowItem;
    m_my_music->setTitleStyleName ("CommonSingleTitleInverted");
    m_my_music->setLayoutPosition (M::VerticalTopPosition);
    m_my_music->imageWidget()->setImage ("icon-m-content-audio-inverse");
    m_my_music->imageWidget()->setStyleName ("CommonMainIcon");
    m_my_music->setObjectName("MContentItem_pickFromMyMusic");
    m_MainLayout->addItem (m_my_music);
    connect (m_my_music, SIGNAL (clicked ()), SLOT (launchMusicBrowser ()));
#endif

#ifndef MEEGO
    if (needOvi) {
        // "Get more from Ovi store"
        m_ovi_store = new RightArrowItem;
        m_ovi_store->setTitleStyleName ("CommonSingleTitleInverted");
        m_ovi_store->setLayoutPosition (M::VerticalBottomPosition);
        m_ovi_store->imageWidget()->setImage (
                "icon-m-content-ovi-store-inverse");
        m_ovi_store->imageWidget()->setStyleName ("CommonMainIcon");
        m_ovi_store->setObjectName("MContentItem_getMoreFromOviStore");
        m_MainLayout->addItem (m_ovi_store);
        connect (m_ovi_store, SIGNAL (clicked ()), SLOT (launchOviStore ()));
    }
#endif

    spacer = new MSeparator;
    spacer->setStyleName ("CommonItemDividerInverted");
    m_MainLayout->addItem (spacer);


    /*
     * The list with the available sound files.
     */
    m_defaults = new AlertToneDefaults(m_tone, 0);
    m_defaults->filtering()->setEnabled (true);
    m_defaults->filtering()->setFilterMode (
            MListFilter::FilterAsBeginningOfLine);
    m_LiveFilterEditor = m_defaults->filtering()->editor();
    m_MainLayout->addItem(m_defaults);
    connect (m_defaults, SIGNAL (displayEntered ()),
             SLOT (defaultsDisplayEntered ()));

    // We need this stretch to keep the widgets growing in size when too much
    // lines are filtered out from the list.
    m_MainLayout->addStretch ();

    retranslateUi();

    connect (m_LiveFilterEditor, SIGNAL(textChanged()),
            this, SLOT(textChanged ()));

    connect(m_defaults, SIGNAL(defaultItemClicked(const QString &)),
            this, SLOT(defaultItemClicked(const QString &)));
}


void
AlertToneBrowser::defaultsDisplayEntered()
{
    SYS_DEBUG ("Start");

    /*
     * A fix for the NB#198788 - Live filtering text editor loses focus in
     * this scenario
     */
    if (!m_LiveFilterEditor || !m_LiveFilterEditor->isOnDisplay())
        m_defaults->setFocus();

    SYS_DEBUG ("end");
}

/*!
 * This method is called by the framework on language-change,
 * so it is used internally to reload the translations for widgets
 */
void
AlertToneBrowser::retranslateUi()
{
#ifdef HAVE_CONTENT_MANAGER
    m_my_music->setProperty ("title", qtTrId("qtn_sond_pick_music"));
#endif

#ifndef MEEGO
    if (m_ovi_store)
        m_ovi_store->setProperty("title", qtTrId("qtn_sond_store"));
#endif

    //% "Ringtones"
    if (m_TitleLabel)
        m_TitleLabel->setText (qtTrId("qtn_sond_ringtones"));

    if (m_DoneAction)
        //% "Done"
        m_DoneAction->setText (qtTrId("qtn_comm_command_done"));

    if (m_CancelAction)
        //% "Cancel"
        m_CancelAction->setText (qtTrId("qtn_comm_cancel"));
}
void
AlertToneBrowser::performCancel ()
{
    SYS_DEBUG ("");
    stopPlayingSound ();
    m_defaults->toneChanged ();
}

void
AlertToneBrowser::performAccept ()
{
    SYS_DEBUG ("");
    stopPlayingSound ();

    if (!currSelectedFile.isEmpty()) {
        #ifdef COPY_SOUND_FILES
        if (SoundSettings::isTemporaryFile(currSelectedFile)) {
            currSelectedFile = SoundSettings::saveFile(currSelectedFile);
            SYS_WARNING ("copy at '%s'", SYS_STR(currSelectedFile));
        }
        #endif

        m_tone->set(currSelectedFile);
    }
}

void
AlertToneBrowser::cancel()
{
    performCancel ();
    emit closePage();
}

void
AlertToneBrowser::accept()
{
    performAccept ();
    emit closePage();
}

void
AlertToneBrowser::launchMusicBrowser()
{
    /*
     * Well, we should stop the playback now...
     */
    stopPlayingSound ();

#ifdef HAVE_CONTENT_MANAGER
    SYS_DEBUG ("launching content picker...");

    #ifndef USE_CONTENT_ITEM_SHEET
    if (!m_MusicBrowser) {
        ContentItemsPage *page = new ContentItemsPage (this);
        page->enableConfirmationButton (false);

        page->setContentTypes (
                QStringList () <<
                  "http://www.tracker-project.org/temp/nmm#MusicPiece");
        page->selectItem (m_tone->trackerId ());

        m_MusicBrowser = page;

        page->setObjectName ("SelectSingleContentItemPage_musicBrowser");
        page->setStyleName ("CommonApplicationPageInverted");
        page->setCommonLayoutSuffix ("Inverted");

        connect (page, SIGNAL (backButtonClicked ()),
                 SLOT (browserBackButtonClicked ()));
        connect (page, SIGNAL (itemClicked (const QString&)),
                 SLOT (selectingMusicItem (const QString&)));
    }

    m_MusicBrowser->appear (MSceneWindow::DestroyWhenDismissed);
    #else
    QStringList        contentTypes;

    contentTypes << "http://www.tracker-project.org/temp/nmm#MusicPiece";
    m_selectedItems.clear();

    m_MusicBrowser = new ContentItemsSheet ();
    m_MusicBrowser->initialize (m_selectedItems, true);
    m_MusicBrowser->setInvertedLayout (true);
    m_MusicBrowser->setContentTypes (contentTypes);

    // It seems that this signal is never received... somehow.
    //connect (m_MusicBrowser, SIGNAL (itemClicked (const QString&)),
    //        SLOT (selectingMusicItem (const QString&)));

    connect (m_MusicBrowser, SIGNAL (doneClicked()),
            SLOT (contentItemsSheetDoneClicked ()));

    connect (m_MusicBrowser, SIGNAL (disappeared()),
            SLOT (deleteMusicBrowser()), Qt::QueuedConnection);

    m_MusicBrowser->appear(
            MApplication::activeApplicationWindow(), 
            MSceneWindow::KeepWhenDone);
    #endif
#endif
}

#ifdef USE_CONTENT_ITEM_SHEET
void 
AlertToneBrowser::contentItemsSheetDoneClicked ()
{
    SYS_WARNING ("m_selectedItems = %s", SYS_STR(m_selectedItems.join(";")));
    if (m_selectedItems.isEmpty()) {
        SYS_WARNING ("No selected item.");
        return;
    }

    selectingMusicItem (m_selectedItems[0]);
}

void
AlertToneBrowser::deleteMusicBrowser()
{
    if(m_MusicBrowser)
    {
        delete m_MusicBrowser;
        m_MusicBrowser = 0;
    }
}
#endif

void
AlertToneBrowser::launchOviStore()
{
    static const char OviIf[] = "com.nokia.OviStoreClient";

    /*
     * Well, we should stop the playback now...
     */
    stopPlayingSound ();

    QDBusInterface OviStore (OviIf, "/", OviIf);
    OviStore.asyncCall ("LaunchWithKeyword", QString ("ringtones"));
}

/*
 * NOTE: This method will not start the playing of the sound file any more.
 */
void
AlertToneBrowser::setAlertTone (
        const QString &fname,
        bool           setGui)
{
    SYS_DEBUG ("*** fname  = %s", SYS_STR(fname));
    SYS_DEBUG ("*** setGui = %s", SYS_BOOL(setGui));

    currSelectedFile = fname;

    if (setGui) {
        /*
         * The nice name is most probably already in the cache. If not the model
         * will do receive a signal.
         * FIXME: how could we test this? Do we need to teszt what happens when
         * this item is not cached yet?
         */
        m_defaults->selectAndScroll (fname,
                TrackerConnection::instance()->niceNameFromFileName(fname));
    }
}

/*!
 * Selects the given item in the browser. This method is going to work even if 
 * called before the widget is shown.
 */
void
AlertToneBrowser::set (
        const QVariant &filename)
{
    currSelectedFile = filename.toString();
    m_tone->set(filename);
}

/*!
 * This slot is called when the text in the live filtering text editor has been
 * changed.
 */
void
AlertToneBrowser::textChanged ()
{
    bool     visible;
    QString  text;

    visible = m_LiveFilterEditor->isOnDisplay();
    if (!m_MainLayout)
        return;

    text = m_LiveFilterEditor->text();

    if (visible && text.isEmpty()) {
         /*
          * Removing the filter editor from the layout.
          */
        m_defaults->setFocus();
        m_LiveFilterEditor->setVisible (false);
        m_MainLayout->removeItem (m_LiveFilterEditor);
    } else if (!visible && !text.isEmpty()) {
        /*
         * Adding the filter editor widget to the layout.
         */
        m_MainLayout->insertItem (filterEditorPosition, m_LiveFilterEditor);
        m_LiveFilterEditor->setVisible (true);
        m_LiveFilterEditor->setFocus();
    }


    m_MainLayout->invalidate ();
    m_defaults->setFilterText (m_LiveFilterEditor->text());
    update ();
}

/*
 * This method will start the playback of a given file. If the player is already
 * playing the very same file the playback will be started.
 */
void
AlertToneBrowser::startPlayingSound (
        const QString &filename)
{
    SYS_DEBUG("");
    bool playingTheSame = false;

    if (m_preview)
        playingTheSame = m_preview->fname() == filename;

    SYS_DEBUG ("playingTheSame = %s", SYS_BOOL(playingTheSame));

    /*
     * If we are already playing the same sound we stop the playback. This is
     * coming from the UI specification: "Second tap of the item will stop the
     * preview while the sound is playing.
     */

    delete m_preview;
    m_preview = 0;

    if (! playingTheSame) {
        m_preview = new AlertTonePreview(filename);
    }
}

/*
 * This method will stop the playback unconditionaly.
 */
void
AlertToneBrowser::stopPlayingSound ()
{
    SYS_DEBUG ("** m_preview = %p", m_preview);
    if (m_preview) {
        delete m_preview;
        m_preview = 0;
    }
}

/*
 * This slot is activated when the user clicks on a sound file in the default
 * sound list.
 */
void
AlertToneBrowser::defaultItemClicked (
        const QString &filename)
{
    SYS_DEBUG ("*** filename = %s", SYS_STR(filename));
    /*
     * The sheet needs to know we have a valid file, the 'done' button should be
     * enabled.
     */
    emit itemSelected ();
    
    // We don't need to set the UI, this signal came from the UI already.
    setAlertTone (filename, false);
    startPlayingSound (filename);
}

/*
 * This slot is activated when the browser back button is clicked. Stopping the
 * playback will fix NB#199653 - Music preview doesn't stop when leaving from
 * "Select from My music" view.
 *
 * FIXME: Maybe watching the back button signal is not robus enough, the
 * playback will not be stopped when some other method is used to page back...
 */
void
AlertToneBrowser::browserBackButtonClicked ()
{
    SYS_DEBUG ("");
    stopPlayingSound ();
#ifdef HAVE_CONTENT_MANAGER
    #ifndef USE_CONTENT_ITEM_SHEET
    m_MusicBrowser->dismiss ();
    m_MusicBrowser = 0;
    #endif
#endif
}

/*
 * This slot is activated when the user selects one item from the content
 * picker.
 */
void
AlertToneBrowser::selectingMusicItem (
        const QString &item)
{
    SYS_WARNING ("----------------------------------------------------");
    SYS_WARNING ("*** item = %s", SYS_STR(item));
    QString fname =
        TrackerConnection::instance ()-> trackerIdToFilename (item);

    if (fname.isEmpty()) {
        SYS_WARNING ("TrackerID '%s' is not valid.", SYS_STR(item));
        stopPlayingSound ();
        return;
    }

    SYS_DEBUG ("*** trackerID = %s", SYS_STR(item));
    SYS_DEBUG ("*** fname     = %s", SYS_STR(fname));
    
    /*
     * The sheet needs to know we have a valid file, the 'done' button should be
     * enabled.
     */
    emit itemSelected ();

    #ifndef USE_CONTENT_ITEM_SHEET
    if (m_MusicBrowser) {
        m_MusicBrowser->dismiss ();
        m_MusicBrowser = 0;
    }
    #endif

    setAlertTone (fname, true);
    startPlayingSound (fname);
}

/*!
 * This virtual method is called when the MApplicationPage for the widget is
 * already there, so we can initialize it.
 */
void
AlertToneBrowser::polishEvent ()
{
    QGraphicsWidget  *parent;
    MApplicationPage *page = 0;

    SYS_DEBUG ("");
    /*
     * Just a protection about double adding the actions.
     */
    if (m_DoneAction)
        return;

    MWindow *win = MApplication::activeWindow ();
    if (win) {
        connect (win, SIGNAL(switcherEntered()),
                this, SLOT(stopPlayingSound()));
    }

    /*
     * We need to find the MApplicationPage among our parents.
     */
    parent = parentWidget();
    while (parent) {
        page = qobject_cast <MApplicationPage *>(parent);
        if (page)
            break;
        parent = parent->parentWidget();
    }

    if (!page)
        return;

    /**************************************************************************
     * Hiding the home button and the escape button from the page.
     *
     * Well, we are using a sheet now, but the clock isn't.
     */
    page->setComponentsDisplayMode (
            MApplicationPage::EscapeButton,
            MApplicationPageModel::Hide);
    page->setComponentsDisplayMode (
            MApplicationPage::HomeButton,
            MApplicationPageModel::Hide);

    //% "Done"
    m_DoneAction = new MAction(qtTrId("qtn_comm_command_done"), this);
    m_DoneAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_DoneAction);
    connect(m_DoneAction, SIGNAL(triggered()), SLOT(accept()));

    //% "Cancel"
    m_CancelAction = new MAction(qtTrId("qtn_comm_cancel"), this);
    m_CancelAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_CancelAction);
    connect(m_CancelAction, SIGNAL(triggered()), SLOT(cancel()));
}

