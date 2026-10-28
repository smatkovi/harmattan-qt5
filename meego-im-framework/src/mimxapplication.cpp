#include "mimxapplication.h"
#include "mimpluginsproxywidget.h"

#include <QDebug>

#ifdef HAVE_MEEGOGRAPHICSSYSTEM
#include <QtMeeGoGraphicsSystemHelper/QMeeGoGraphicsSystemHelper>
#endif

#include <deque>

#if QT_VERSION >= 0x050000
#include <QAbstractNativeEventFilter>
#include <QX11Info>
#include <cstring>
#include <xcb/xcb.h>
#endif

#include <X11/Xlib.h>
#if QT_VERSION >= 0x050000
#include <X11/Xlibint.h>
#endif

#if QT_VERSION >= 0x050000
/*
 * Qt 5 hands X events over in xcb's format only, where Qt 4 called
 * QApplication::x11EventFilter(XEvent *). Each xcb event becomes an XEvent
 * again, with Xlib's own conversion from the wire format (extensions such
 * as Damage register theirs with the Display, the connection Qt uses),
 * for MImXApplication::x11EventFilter() (as libmeegotouch's MApplication).
 */
class MImX11EventBridge : public QAbstractNativeEventFilter
{
public:
    explicit MImX11EventBridge(MImXApplication *application) : application(application) {}

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *)
    {
        if (eventType != "xcb_generic_event_t")
            return false;
        Display *dpy = QX11Info::display();
        xcb_generic_event_t *event = static_cast<xcb_generic_event_t *>(message);
        const int type = event->response_type & ~0x80;
        if (!dpy || type < 2)    // errors and replies
            return false;

        typedef Bool (*WireToEvent)(Display *, XEvent *, xEvent *);
        WireToEvent convert = XESetWireToEvent(dpy, type, 0);
        XESetWireToEvent(dpy, type, convert);
        if (!convert)
            return false;

        // the 32 bytes of the wire event (xcb puts the full sequence number
        // after them); Xlib reckons the 16-bit sequence number against its
        // own request count, which Qt's requests through xcb leave behind:
        // the one it read last, so that nothing changes
        xEvent wire;
        std::memcpy(&wire, event, sizeof wire);
        wire.u.u.sequenceNumber = static_cast<CARD16>(dpy->last_request_read);
        XEvent xevent;
        std::memset(&xevent, 0, sizeof xevent);
        if (!convert(dpy, &xevent, &wire))
            return false;
        xevent.xany.display = dpy;
        return application->x11EventFilter(&xevent);
    }

private:
    MImXApplication *application;
};
#endif

namespace
{
    bool configureForCompositing(QWidget *w)
    {
        if (not w) {
            return false;
        }

        w->setAttribute(Qt::WA_OpaquePaintEvent);
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->setAutoFillBackground(false);
        // Be aware that one cannot verify whether the background role *is*
        // QPalette::NoRole - see QTBUG-17924.
        w->setBackgroundRole(QPalette::NoRole);

        if (MIMApplication::instance() && not MIMApplication::instance()->selfComposited()) {
            // Careful: This flag can trigger a call to
            // qt_x11_recreateNativeWidgetsRecursive
            // - which will crash when it tries to get the effective WId
            // (as none of widgets have been mapped yet).
            w->setAttribute(Qt::WA_TranslucentBackground);
        }

        return true;
    }
}

MImXApplication::MImXApplication(int &argc, char** argv) :
    MIMApplication(argc, argv),
    mCompositeExtension(),
    mDamageExtension(),
    mSelfComposited(false),
    mManualRedirection(false),
    mBypassWMHint(false),
    mBackgroundSuppressed(false),
    mUnconditionalShow(false),
    mPassThruWindow(),
    mRemoteWindow()
{
    parseArguments(argc, argv);

    mPassThruWindow.reset(new MPassThruWindow(this));
    mPluginsProxyWidget.reset(new MImPluginsProxyWidget(mPassThruWindow.get()));

#ifdef HAVE_MEEGOGRAPHICSSYSTEM
    QMeeGoGraphicsSystemHelper::setSwitchPolicy(QMeeGoGraphicsSystemHelper::NoSwitch);
#endif

    configureWidgetsForCompositing();

    connect(this, SIGNAL(aboutToQuit()),
            this, SLOT(finalize()),
            Qt::UniqueConnection);
#if QT_VERSION >= 0x050000
    installNativeEventFilter(new MImX11EventBridge(this));
#endif
}

MImXApplication::~MImXApplication()
{
}

void MImXApplication::finalize()
{
    // Cannot destroy QWidgets or QPixmaps during MIMApplication destruction.
    // Hence the finalize handler that runs before the d'tor.
    mPluginsProxyWidget.reset();
    mPassThruWindow.reset();
    mRemoteWindow.reset();
}

void MImXApplication::parseArguments(int &argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        QLatin1String arg(argv[i]);

        if (arg == "-manual-redirection") {
            mManualRedirection = true;
        } else if (arg == "-bypass-wm-hint") {
            mBypassWMHint = true;
        } else if (arg == "-use-self-composition") {
            mSelfComposited = mCompositeExtension.supported(0, 2) && mDamageExtension.supported();
        } else if (arg == "-unconditional-show") {
            mUnconditionalShow = true;
        }
    }
}

bool MImXApplication::x11EventFilter(XEvent *ev)
{
    handleTransientEvents(ev);
    handleRemoteWindowEvents(ev);
    handlePassThruMapEvent(ev);
#if QT_VERSION >= 0x050000
    return false;
#else
    return QApplication::x11EventFilter(ev);
#endif
}

void MImXApplication::handleTransientEvents(XEvent *ev)
{
    if (not mRemoteWindow.get()) {
        return;
    }

    if (mRemoteWindow->wasIconified(ev) || mRemoteWindow->wasUnmapped(ev)) {
        qDebug() << "MIMApplication" << __PRETTY_FUNCTION__
                 << "Remote window was destroyed or iconified - hiding.";

        Q_EMIT remoteWindowChanged(0);
        Q_EMIT remoteWindowGone();
        mRemoteWindow.reset();
    }
}

void MImXApplication::handleRemoteWindowEvents(XEvent *event)
{
    if (not mRemoteWindow.get()) {
        return;
    }

    mRemoteWindow->handleEvent(event);
}

void MImXApplication::handlePassThruMapEvent(XEvent *ev)
{
    if (ev->type != MapNotify)
        return;

    if (ev->xmap.window != mPassThruWindow->effectiveWinId())
        return;

    if (not mRemoteWindow.get()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "No remote window found, but passthru window was mapped.";
        return;
    }

#if QT_VERSION >= 0x050000
    // Qt 5's xcb removes WM_TRANSIENT_FOR whenever it maps a plain window:
    // the application's window again (mcompositor follows the change)
    mRemoteWindow->setIMWidget(mPassThruWindow->window());
#endif
    mRemoteWindow->resetPixmap();
}

void MImXApplication::setTransientHint(WId newRemoteWinId)
{
    if (0 == newRemoteWinId) {
        return;
    }

    if (mRemoteWindow.get() && mRemoteWindow->id() == newRemoteWinId) {
        return;
    }

    const bool wasRedirected(mRemoteWindow.get() && mRemoteWindow->isRedirected());

    mRemoteWindow.reset(new MImRemoteWindow(newRemoteWinId, this));
    mRemoteWindow->setIMWidget(mPassThruWindow->window());

    connect(mRemoteWindow.get(), SIGNAL(contentUpdated(QRegion)),
            this,                SLOT(updatePassThruWindow(QRegion)));

    if (wasRedirected) {
        mRemoteWindow->redirect();
    }

    Q_EMIT remoteWindowChanged(mRemoteWindow.get());
}

bool MImXApplication::selfComposited() const
{
    return mSelfComposited;
}

bool MImXApplication::manualRedirection() const
{
    return mManualRedirection;
}

bool MImXApplication::bypassWMHint() const
{
    return mBypassWMHint;
}

bool MImXApplication::unconditionalShow() const
{
    return mUnconditionalShow;
}

void MImXApplication::setSuppressBackground(bool suppress)
{
    mBackgroundSuppressed = suppress;
}

QWidget *MImXApplication::passThruWindow() const
{
    return mPassThruWindow.get();
}

QWidget* MImXApplication::pluginsProxyWidget() const
{
    return mPluginsProxyWidget.get();
}

const QPixmap &MImXApplication::remoteWindowPixmap()
{
    if (not mRemoteWindow.get()
            || mBackgroundSuppressed
            || not mSelfComposited) {
        static const QPixmap empty;
        return empty;
    }

    return mRemoteWindow->windowPixmap();
}

void MImXApplication::updatePassThruWindow(const QRegion &region)
{
    if (region.isEmpty() || remoteWindowPixmap().isNull()) {
        qDebug() << "Skipping update request for passthru window.";
        return;
    }

    mPassThruWindow->updateFromRemoteWindow(region);
}

void MImXApplication::visitWidgetHierarchy(WidgetVisitor visitor,
                                           QWidget *widget)
{
    std::deque<QWidget *> unvisited;
    unvisited.push_back(widget);

    // Breadth-first traversal of widget hierarchy, until no more
    // unvisited widgets remain. Will find viewports of QGraphicsViews,
    // as QAbstractScrollArea reparents the viewport to itself.
    while (not unvisited.empty()) {
        QWidget *current = unvisited.front();
        unvisited.pop_front();

        // If true, then continue walking the hiearchy of current widget.
        if (visitor(current)) {
            // Mark children of current widget as unvisited:
            Q_FOREACH (QObject *obj, current->children()) {
                if (QWidget *w = qobject_cast<QWidget *>(obj)) {
                    unvisited.push_back(w);
                }
            }
        }
    }
}

void MImXApplication::configureWidgetsForCompositing()
{
    visitWidgetHierarchy(configureForCompositing, mPassThruWindow.get());
}

#ifdef UNIT_TEST
MImRemoteWindow *MImXApplication::remoteWindow() const
{
    return mRemoteWindow.get();
}
#endif
