# meego-camera

The Nokia N9's camera, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`) on GStreamer and libcamera, for
the MeeGo session on Maemo Leste, Arch Linux ARM and FreeBSD and for
Hildon. Nokia's camera is closed source; this app imitates its look and
behaviour, it contains none of Nokia's code or files (nor Nemo's: the
code is new).

- **The viewfinder** full screen, in landscape as the N9's.
- **At the side**: the switch between photo and video above, the shutter
  key in the middle (a moment of white for a photo; for a video it starts
  and stops it), the last picture below (a tap shows it in
  `meego-gallery`, a video in `meego-video`).
- **The other camera** at the top left, where there are two; **a video's
  time** at the top while it is made.
- **No camera**: the N9's camera in its sleep and "No camera" (or why it
  does not start), no keys.
- Space, Enter or a camera key take the picture too. The camera is off
  while the application is hidden or suspended.

The cameras are libcamera's (its GStreamer source `libcamerasrc`, at
1280×720) where that plugin is installed, else the V4L2 devices
(`v4l2src`, any format `decodebin` decodes), both found through
GStreamer's device monitor; `MEEGO_CAMERA_SOURCE` gives another source as
a gst-launch description (a test: `videotestsrc is-live=true`). The
picture comes through an appsink as RGB images into Qt Quick (no GL path
needed); a second appsink keeps the latest whole frame, and a photo is
that frame as a JPEG (`gst_video_convert_sample` with `jpegenc`). A video
is the same pipeline with an encoding branch: H.264 (x264, OpenH264 or a
V4L2 encoder) in MP4 with AAC, else VP8 in WebM with Opus or Vorbis, the
sound from `autoaudiosrc` (`MEEGO_CAMERA_AUDIOSRC` another, `none` none).
They are saved where the N9 kept them, `~/MyDocs/DCIM` if there is a
`~/MyDocs`, else `~/DCIM` (or `MEEGO_CAMERA_DIR`), named by the time.

## The look: the N9's theme

Through `image://theme/`: the shutter key's, the mode switcher's, the
review button's and the time's backgrounds (`meegotouch-camera-*`), the
camera icons, the components' own. These are Nokia's (the "blanco" theme
of one's own N9 or N950) and are not in this repository.

## Building

Qt 5 (Quick, Concurrent), GStreamer 1.0 with its base plugins' libraries
(`gstreamer-1.0`, `gstreamer-app-1.0`, `gstreamer-video-1.0`) and
qt-components' `com.nokia.meego` 2.0 at run time; for the cameras
libcamera's GStreamer plugin or GStreamer's V4L2 plugin (good plugins),
for photos `jpegenc` (good), for videos `x264enc` (ugly) or `openh264enc`
(bad) with `mp4mux` (good) and `avenc_aac` (libav) or `fdkaacenc`, or
`vp8enc` and `webmmux` (good) with `opusenc` or `vorbisenc` (base):

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
