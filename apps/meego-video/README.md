# meego-video

The Nokia N9's videos, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`) on GStreamer, for the MeeGo
session on Maemo Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's
"Video & TV" is closed source; this app imitates its look and behaviour,
it contains none of Nokia's code or files (nor Nemo's: the code is new).

- **Videos** (dark, as the N9's): the newest first, each with a picture
  of it (a frame a tenth into it), its length on the N9's duration plate
  and its title; a tap plays it, a long press deletes it (to the
  desktop's trash).
- **Playing**: full screen, at once; a tap shows the controls for a few
  seconds: the title, the position to drag with the elapsed and the whole
  time, back and play/pause; at the end they stay.

The video is GStreamer's playbin with its frames through an appsink into
a Qt Quick item (RGB images, so it needs no GL path between GStreamer and
Qt), the sound to the default output (`MEEGO_VIDEO_AUDIOSINK` for another,
e.g. `fakesink sync=true` in a test). The videos are those below the
videos folder (`XDG_VIDEOS_DIR`), `~/DCIM` and `~/MyDocs/DCIM`, or the
folders in `MEEGO_VIDEO_DIR`: MP4, M4V, MKV, WebM, AVI, MOV, OGV, 3GP, as
far as the installed GStreamer plugins decode them. The pictures and the
lengths are made once per file and cached in
`~/.cache/meego/meego-video/thumbnails`. `meego-video video.mp4` plays it.

## The look: the N9's theme

Through `image://theme/`: the video placeholder and duration plate, the
tool bar's media icons, the components' own. These are Nokia's (the
"blanco" theme of one's own N9 or N950) and are not in this repository.

## Building

Qt 5 (Quick, Concurrent), GStreamer 1.0 with its base plugins' libraries
(`gstreamer-1.0`, `gstreamer-app-1.0`, `gstreamer-video-1.0`,
`gstreamer-pbutils-1.0`) and qt-components' `com.nokia.meego` 2.0 at run
time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
