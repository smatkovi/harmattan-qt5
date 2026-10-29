# harmattan-qt5

The open source parts of the Nokia N9's user interface (MeeGo 1.2
Harmattan), ported from Qt 4 to Qt 5, to run on Maemo Leste and FreeBSD
(the F(x)tec Pro1X port in smatkovi/pro1x-maemo-leste): the window manager
`mcompositor`, the home screen `meegotouchhome`, the system UI `system-ui`
and the MeeGo Touch framework `libmeegotouch` they are built on, with the
libraries those need.

Packages: for FreeBSD 15 (arm64) in
[pro1x-maemo-leste's FreeBSD releases](https://github.com/smatkovi/pro1x-maemo-leste/releases)
(`harmattan-<directory>`, built by its `freebsd/hildon/harmattan.sh`; not
run on FreeBSD yet), for Maemo Leste (arm64) from its
`meego/50-harmattan-qt5-debs.sh`; Arch Linux ARM builds them in its image
(`arch/30-meego.sh`).

Each directory starts as the source package of Nokia's Harmattan PR1.2
open source release (the first commit), unchanged; the port follows in
the commits after it.

| Directory | Source package |
| --- | --- |
| `libmlocale` | libmlocale 0.3.20-1+0m7 |
| `libmeegotouch` | libmeegotouch 0.24.21-1+0m7 |
| `mcompositor` | mcompositor 1.1.34-1+0m7 |
| `meegotouchhome` | meegotouchhome 1.2.20-1+0m7 |
| `system-ui` | system-ui 1.2.24-2+0m7 |
| `meegotouchtheme` | meegotouchtheme 0.24.13-1+0m7 (the LGPL "base" theme) |
| `contextkit` | contextkit 0.5.41+0m7 |
| `qmsystem2` | qmsystem2 1.4.3+0m7 |
| `mce-dev` | mce-dev 1.11.0+0m7 (MCE's D-Bus names, headers only) |
| `libdsme` | libdsme 0.61.6+0m6 |
| `meegofeedbackd` | meegofeedbackd 0.10.7-5+0m6 |
| `libresourceqt` | libresourceqt 1.41+0m7 |
| `libcontentaction` | libcontentaction 0.1.71+0m7 |
| `duicontrolpanel` | duicontrolpanel 0.14.7.1+0m7 |
| `meegotouch-controlpanelapplets` | meegotouch-controlpanelapplets 0.21.10-1+0m7 (the settings applets) |
| `libconnsettings` | libconnsettings 0.29+0m6 |
| `meego-im-settings-applet` | meego-im-settings-applet 0.80.12+0m7 (text input settings) |
| `meego-im-framework` | meego-im-framework 0.80.8.harmattan19+0m7 (the input method framework, Maliit) |
| `meego-keyboard` | meego-keyboard 0.7.21-5+0m7 (the N9's virtual keyboard, BSD) |
| `libmeegoimengine` | libmeegoimengine 0.4.11-1+0m7 (the keyboard's word engine interface) |

Not open, and not here: the N9's own extensions (`mcompositor-swipe`,
`meegotouchhome-nokia` with the Events view, `meegotouch-systemui-nokia`
with the screen lock and the volume bar) and its "blanco" theme; those
are written anew elsewhere or come from one's own device.

Licences: as each package states (mostly LGPL 2.1).

## Status

| Directory | Qt 5 |
| --- | --- |
| `libmlocale` | builds |
| `libmeegotouch` | builds; applications run with the N9's theme; without libmaliit it tells Qt 5's input context its windows' angle (content orientation) and learns the keyboard's area from it |
| `contextkit` | builds; providers and subscribers talk over D-Bus |
| `mcompositor` | builds; manages and composites MeeGo Touch and plain X11 windows, `mdecorator` frames the latter (GLX; the unit tests don't build yet) |
| `libcontentaction` | builds |
| `meegotouchhome` | runs, with the N9's three pages written anew (its home, 1.3.2, was not released): Events (the system UI's notifications, newest first, a tap runs one's action and dismisses it; no feeds), Applications (every application in one grid, panned vertically) and Open applications (the switcher's windows in a vertical grid), side by side in a ring and changed by a sideways swipe; `MEEGOTOUCHHOME_LAYOUT=netbook` keeps the release's own layout (quick launch bar, launcher, switcher) |
| `system-ui` | builds without QmSystem2/libresourceqt/libngf; sysuid draws the status bar that the applications show and opens the status indicator menu with the plugins of `statusmenu-plugins` |
| `duicontrolpanel` | the settings application runs with the N9's categories (`DCP_ROOT` for a staging directory) |
| `meegotouch-controlpanelapplets` | about, reset, warranty, test, display, battery, offline, USB and the sound settings ("Alert Tones": libprofile, as Leste's profiled has it, GStreamer 1.0 for the preview; without QtMobility, timed and QtSparql) |
| `qmsystem2` | builds; the battery from UPower or the kernel, time without timed, the heartbeat without iphbd |
| `libconnsettings` | builds (plain C, on Leste's osso-ic headers) |
| `mcompositor/plugins/swipe` | new: the N9's swipe (minimize from the sides, close from the top, sneak peek) |
| `meegotouchtheme/pro1x` | new: the Pro1X theme, blanco dark everywhere (pages #000000 for the AMOLED panel, light texts; only the palette, blanco's images are inherited) |
| `ngfd-meego` | the configuration for Nemo's ngfd (built from upstream), which plays the N9's event sounds with the N9's own event definitions; system-ui with `DEFINES+=HAVE_LIBNGF` sends its notifications' sounds there |
| `statusmenu-plugins` | new: the status indicator menu's rows (the N9's were not released): the volume of PulseAudio's default output, the internet connection from NetworkManager (with the WLAN switch) and Bluetooth from BlueZ, light on black as the N9's; `M_APPLICATION_EXTENSION_DIR` and `M_APPLICATION_EXTENSION_DATA_DIR` let libmeegotouch find them in a stage |
| `meegofeedbackd` | runs; MFeedback sounds (the theme's `feedbacks/*/audio.wav`) through `meegofeedback-pulse`, a PulseAudio backend written anew (Nokia's were not released) |
| `meegofeedback-reactionmaps` | runs: the keyboard paints its reaction maps, meegofeedbackd's XRecord source plays a key's click on the touch itself; install one source only (the kernel one reads the touchscreen's events directly, multitouch protocol A only), meegofeedbackd loads every one in `sources/`; open libmeegotouch paints no reaction maps for its plain controls |
| `meego-im-framework` | builds (legacy mode, as Harmattan: `CONFIG+=enable-legacy CONFIG+=enable-contextkit`); meego-im-uiserver shows the plugins for MeeGo Touch applications, mcompositor composites its window over theirs, text goes back; Qt 5 applications reach it through `platforminputcontext` (Qt 5.1's Maliit platform input context, `QT_IM_MODULE=maliit`); the Qt 4 input context, the GTK+ bridge and the QtQuick 1 plugin base are not built |
| `libmeegoimengine` | builds, with its open dummy engine (the N9's word engine, with correction and prediction, was not released) |
| `meego-keyboard` | runs: the N9's keyboard in meego-im-uiserver, landscape, with the N9's theme; typing reaches MeeGo Touch applications, which move the editor out of its way; Qt 5 dropped the primary touch point, `common/touchcompat.h` keeps it; `M_VKB_LAYOUTS_DIR` for a staged test; with its key clicks through the reaction maps; without the QtQuick 1 keyboard |
| `meego-im-settings-applet` | the "Text input" settings page runs (active and installed input methods, the keyboard's settings) |
| `libresourceqt` | not yet |

Tried on a PC (Xvfb, Mesa llvmpipe), not yet on a device.

What Qt 5 changed for these, in short (the commit messages have the
details): X events arrive as xcb events, so each becomes an XEvent again
for the `x11EventFilter()`s; Xlib's event queue stays empty, so
`XIfEvent()`-style waits look into Qt's queue; QPixmap has no X pixmap
behind it any more; Xlib's `Bool` macro breaks moc's code; `mmoc` is
the moc again.

## Applications in the N9's look

New, not ports: the N9's own applications were closed source. Written in
QML with the Harmattan Qt Quick components (`com.nokia.meego 2.0`, as
[meego-clock](https://github.com/smatkovi/meego-clock)), each in
`apps/<name>` with a README; they draw with the N9's "blanco" theme from
one's own device (not in this repository). Tried on a PC under Xvfb with
qt-components built for Qt 5.15.

| Directory | State |
| --- | --- |
| `apps/meego-calendar` | runs: the month on the N9's grid (today, the selected day, event dots, swipe to change the month), the day's events beside or below it, the upcoming events, an event's details, the editor sheet with the date and time pickers, repetitions, colours; its own SQLite store; alerts saved, not rung yet |
| `apps/meego-music` | runs: the covers' mosaic with the song that plays, all songs, artists, albums (as covers), genres, the smart playlists (recently added and played, most played), now playing (cover, position to drag, shuffle, repeat, previous/play/next); GStreamer's playbin and discoverer (tags, embedded covers), cached |
| `apps/meego-phone` | runs against a stand-in ModemManager (`tests/fake-modemmanager.py`): the dialler with the latest calls, the call history, the call screen (calling, incoming with answer and reject, the call's time, mute, loudspeaker, tones, end); "No modem" without one; untested with a real modem |
| `apps/meego-messages` | runs against a stand-in ModemManager (`tests/fake-modemmanager.py`): the conversations (one per number, unread counts), a conversation in the N9's bubbles with sending and its state, messages arriving live or stored on the modem before, new message, forward, delete; its own SQLite store; untested with a real modem |
| `apps/meego-mail` | runs against a stand-in server (`tests/fake-mailserver.py`, plain IMAP and SMTP): the folder with unread messages, folders, a message (encoded words, quoted-printable, multipart, HTML as text, attachments' names), reply with the quote, reply to all, forward, delete, mark unread, sending, the account sheet with the providers' servers; its own IMAP, SMTP and MIME; TLS untested against a real server |
| `apps/meego-contacts` | runs: the contacts under their letters with the letter scroller and search, the card (calls, messages and mails through the other apps), the editor sheet, import of vCard 2.1/3.0/4.0 files with photos (twice gives each once); one vCard file per contact |
| `apps/meego-gallery` | runs: all pictures as the N9's gapless grid, the albums (folders), a picture full screen (swipe, double-tap zoom, details, delete to the trash) |
| `apps/meego-video` | runs: the videos with their pictures (a frame a tenth in) and lengths, playing full screen with the controls (position, play/pause); playbin through an appsink into Qt Quick (no GL path needed); VP8/WebM, H.264/MP4 and Theora tried |
| `apps/meego-notes` | runs: the notes under the N9's yellow header, the newest first (the first line as title, a little of the rest, the time), search, the editor sheet (save, cancel), delete to the trash; one plain text file per note, changes from outside seen at once |
| `apps/meego-calculator` | runs: the calculation line in the N9's orange (shrinking as it grows, backspace, copy and paste), the keys with the N9's yellow pressed squares, portrait and landscape layouts, one operation at a time, the hardware keyboard |
| `apps/meego-camera` | runs with GStreamer's test source (`MEEGO_CAMERA_SOURCE`): the viewfinder, photos (JPEG of the whole frame), videos (H.264 and AAC in MP4, the time while recording), the mode switch, the last picture opening the gallery or the videos, "No camera" without one; libcamera (`libcamerasrc`) and V4L2 cameras found through GStreamer's device monitor, untested with a real camera |
| `apps/meego-browser` | runs with a stand-in web view (`tests/FakeView.qml`): the address field (addresses, files, searches, stop, progress), the start view with bookmarks and recent pages, back and forward, bookmarking; says so without a web engine; the Qt WebEngine and Qt WebKit views are written but untested (neither is packaged for Qt 5 on the PC) |

## Building on a PC

Each directory installs into a stage (`DESTDIR`), and the next one is
built against it: `libmlocale`, `libmeegotouch` (qmake), `contextkit`
(`./autogen.sh --no-configure && ./configure --prefix=/usr --disable-doc
--disable-coverage`; needs tinycdb), `mcompositor` (qmake with
`CONFIG+=NODOCS`). The stage's `bin` goes on the `PATH` (for `mmoc`),
its `lib/pkgconfig` on `PKG_CONFIG_PATH`, its mkspecs features on
`QMAKEFEATURES`.

To run without installing, libmeegotouch reads the theme from
`M_THEME_DIR`/`M_THEME_NAME` and its target configurations from
`M_TARGETS_CONF_DIR`, extra translations from `M_TRANSLATION_DIR`;
contextkit reads its registry from
`CONTEXT_PROVIDERS`, `CONTEXT_TYPES`, `CONTEXT_CORE_TYPES` and
`CONTEXT_CORE_DECLARATIONS`.
