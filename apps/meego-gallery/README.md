# meego-gallery

The Nokia N9's gallery, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`), for the MeeGo session on Maemo
Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's gallery is
closed source; this app imitates its look and behaviour, it contains none
of Nokia's code or files (nor Nemo's: the code is new).

- **All** (dark, as the N9's): every picture as a square thumbnail without
  gaps, the newest first.
- **Albums**: the folders, each with its newest picture and its count; an
  album's pictures as the same grid.
- **A picture** full screen: a sideways swipe to the next or the one
  before, a double tap zooms in where it was tapped and out again (the
  zoomed picture can be dragged), a tap hides or shows the bars; the name
  and its place in the list, details (date, size in pixels and bytes,
  folder), delete (to the desktop's trash).

The pictures are those below the pictures folder (`XDG_PICTURES_DIR`),
`~/DCIM` and `~/MyDocs/DCIM` (a camera's), or the folders in
`MEEGO_GALLERY_DIR` (colon-separated): JPEG, PNG, GIF, BMP and WebP,
hidden folders (thumbnail caches) left out; found in the background.
`meego-gallery picture.jpg` opens it full screen.

## The look: the N9's theme

Through `image://theme/`: the tool bar's grid and gallery icons as tabs,
the components' own. These are Nokia's (the "blanco" theme of one's own
N9 or N950) and are not in this repository.

## Building

Qt 5 (Quick, Concurrent) and qt-components' `com.nokia.meego` 2.0 at run
time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
