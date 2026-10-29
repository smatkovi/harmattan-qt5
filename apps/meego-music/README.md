# meego-music

The Nokia N9's music player, written anew in QML with the MeeGo Harmattan
Qt Quick components (`com.nokia.meego 2.0`) on GStreamer, for the MeeGo
session on Maemo Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's
player is closed source; this app imitates its look and behaviour, it
contains none of Nokia's code or files (nor Nemo's: the code is new).

- **Main view** (dark, as the N9's): the covers of the music played
  lately (then the newest) as a mosaic of nine, with the song that plays
  over it (a tap opens it); beside it (landscape) or below it (portrait)
  All songs, Artists, Albums, Genres and Playlists with their counts. The
  tool bar looks for new music, shuffles all, and goes to the song that
  plays.
- **Lists**: all songs (title, artist and album, length), the artists
  (cover, albums and songs), the albums as covers (all or an artist's;
  an album's songs by track number under its cover), the genres, and the
  N9's smart playlists: recently added, recently played, most played. A
  tap plays from there; shuffle plays a list in a random order.
- **Now playing**: the cover large, the song, artist and album, its place
  in the queue, shuffle and repeat, the position to drag with the elapsed
  and the whole time; previous (back to the start after 3 s, as the N9),
  play or pause, next in the tool bar. A song that cannot be played is
  skipped.

The library is the music folder (`XDG_MUSIC_DIR`, or `MEEGO_MUSIC_DIR`)
with its subfolders: MP3, Ogg Vorbis, Opus, FLAC, AAC/M4A, WAV, WMA. The
tags and covers come from GStreamer's discoverer (a cover in the file's
tags, else `cover`, `folder` or `front` beside it) and are cached in
`~/.cache/meego/meego-music` with the play counts; files that did not
change are not read again. `MEEGO_MUSIC_AUDIOSINK` gives playbin another
output (e.g. `fakesink sync=true` for a test).

## The look: the N9's theme

Through `image://theme/`: the tool bar's media icons, the library's icons
(`icon-m-content-*-inverse`, `icon-m-music-video-*`), the drill-down
arrow, the components' own. These are Nokia's (the "blanco" theme of
one's own N9 or N950) and are not in this repository.

## Building

Qt 5 (Quick, Concurrent), GStreamer 1.0 with its base plugins'
libraries (`gstreamer-1.0`, `gstreamer-pbutils-1.0`, `gstreamer-tag-1.0`),
and qt-components' `com.nokia.meego` 2.0 at run time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
