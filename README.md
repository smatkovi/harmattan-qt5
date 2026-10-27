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
| `libresourceqt` | libresourceqt 1.41+0m7 |
| `libcontentaction` | libcontentaction 0.1.71+0m7 |
| `duicontrolpanel` | duicontrolpanel 0.14.7.1+0m7 |

Not open, and not here: the N9's own extensions (`mcompositor-swipe`,
`meegotouchhome-nokia` with the Events view, `meegotouch-systemui-nokia`
with the screen lock and the volume bar) and its "blanco" theme; those
are written anew elsewhere or come from one's own device.

Licences: as each package states (mostly LGPL 2.1).
