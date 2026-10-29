# meego-notes

The Nokia N9's notes, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`), for the MeeGo session on Maemo
Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's "Notes" is
closed source; this app imitates its look and behaviour, it contains none
of Nokia's code or files (nor Nemo's: the code is new).

- **Notes** (light, under the N9's yellow header): the newest first, each
  with its first line as the title, a little of the rest below it and the
  time it was last changed (the hour today, else the day); a tap opens it,
  a long press deletes it (after asking; to the desktop's trash). With
  none, "No notes" and a button to add one. The tool bar's search shows
  only the notes with all the words written in it.
- **The editor sheet**: the whole sheet is the note's text, Save keeps it
  (only when it changed), Cancel leaves it as it was.

The notes are plain UTF-8 text files, one per note, in
`~/.local/share/meego/meego-notes/notes` (or the folder in
`MEEGO_NOTES_DIR`), named by the time they were made; a file's time is
the note's. Files put there or changed by another program (a sync, an
editor) show at once. `meego-notes --new` starts with a new note.

## The look: the N9's theme

Through `image://theme/`: the tool bar's icons, the sheet, the components'
own. These are Nokia's (the "blanco" theme of one's own N9 or N950) and
are not in this repository.

## Building

Qt 5 (Quick) and qt-components' `com.nokia.meego` 2.0 at run time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
