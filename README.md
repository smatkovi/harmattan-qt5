# harmattan-qt5

The open source parts of the Nokia N9's user interface (MeeGo 1.2
Harmattan), ported from Qt 4 to Qt 5, to run on Maemo Leste and FreeBSD
(the F(x)tec Pro1X port in smatkovi/pro1x-maemo-leste): the window manager
`mcompositor`, the home screen `meegotouchhome`, the system UI `system-ui`
and the MeeGo Touch framework `libmeegotouch` they are built on, with the
libraries those need.

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
| `libmeegotouch` | builds; applications run with the N9's theme |
| `contextkit` | builds; providers and subscribers talk over D-Bus |
| `mcompositor` | builds; manages and composites MeeGo Touch and plain X11 windows, `mdecorator` frames the latter (GLX; the unit tests don't build yet) |
| `libcontentaction` | builds |
| `meegotouchhome` | builds and starts; it is the generic MeeGo home of the source release (1.2.20, quick launch bar, launcher, switcher), not the N9's three-page home (1.3.2, not released) |
| `system-ui` | builds without QmSystem2/libresourceqt/libngf; sysuid draws the status bar that the applications show |
| `duicontrolpanel` | the settings application runs with the N9's categories (`DCP_ROOT` for a staging directory) |
| `meegotouch-controlpanelapplets` | about, reset, warranty and test applets; display, battery, offline and USB wait for QmSystem2, the sound settings for libprofile and GStreamer 1.0 |
| `qmsystem2` | builds; the battery from UPower or the kernel, time without timed, the heartbeat without iphbd |
| `libconnsettings` | builds (plain C, on Leste's osso-ic headers) |
| `mcompositor/plugins/swipe` | new: the N9's swipe (minimize from the sides, close from the top, sneak peek) |
| `meegotouchtheme/pro1x` | new: the Pro1X theme, blanco dark everywhere (pages #000000 for the AMOLED panel, light texts; only the palette, blanco's images are inherited) |
| `ngfd-meego` | the configuration for Nemo's ngfd (built from upstream), which plays the N9's event sounds with the N9's own event definitions; system-ui with `DEFINES+=HAVE_LIBNGF` sends its notifications' sounds there |
| `meegofeedbackd` | runs; MFeedback sounds (the theme's `feedbacks/*/audio.wav`) through `meegofeedback-pulse`, a PulseAudio backend written anew (Nokia's were not released); the N9's tap sounds of plain controls come from reaction maps (`meegofeedback-reactionmaps`), not yet |
| `libresourceqt`, `meego-im-framework`, `meego-keyboard`, `meego-im-settings-applet` | not yet |

Tried on a PC (Xvfb, Mesa llvmpipe), not yet on a device.

What Qt 5 changed for these, in short (the commit messages have the
details): X events arrive as xcb events, so each becomes an XEvent again
for the `x11EventFilter()`s; Xlib's event queue stays empty, so
`XIfEvent()`-style waits look into Qt's queue; QPixmap has no X pixmap
behind it any more; Xlib's `Bool` macro breaks moc's code; `mmoc` is
the moc again.

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
