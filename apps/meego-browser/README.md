# meego-browser

The Nokia N9's web browser, written anew in QML with the MeeGo Harmattan
Qt Quick components (`com.nokia.meego 2.0`) on Qt WebEngine or Qt WebKit,
for the MeeGo session on Maemo Leste, Arch Linux ARM and FreeBSD and for
Hildon. Nokia's browser is closed source; this app imitates its look and
behaviour, it contains none of Nokia's code or files (nor Nemo's: the
code is new).

- **The address field** above: the page's address; Enter opens what is
  written, an address (`example.org`, `localhost:8080`, a file's path) or
  else a search (DuckDuckGo, `MEEGO_BROWSER_SEARCH` another with `%s` for
  the words); stop while a page loads, the load's progress under it.
- **The page**, or **the start view**: the bookmarks, then the pages seen
  lately (a tap opens one, a long press removes a bookmark).
- **The tool bar**: back (from the first page to the start view), forward,
  the start view, the bookmark (on or off), the menu (reload, clear the
  history).
- On a keyboard: Ctrl+L to the address field, F5 reloads, Alt+Left and
  Alt+Right back and forward.

The web view is whichever QML module is installed: Qt WebEngine
(`QtWebEngine 1.10`, Chromium), else Qt WebKit (`QtWebKit 3.0`);
`MEEGO_BROWSER_ENGINE=webkit` or `webengine` chooses one. With neither the
start view says so and the rest still works. A page's new window opens
in the same view; the position, the camera and the microphone are not
given to pages. The bookmarks and the history (the last 300 pages) are
JSON in `~/.local/share/meego/meego-browser`. `meego-browser ADDRESS`
opens it at once.

`tests/FakeView.qml` stands in for a web view in tests
(`MEEGO_BROWSER_VIEW=file:///…/tests/FakeView.qml`): local HTML files as
Qt's rich text, links, back and forward, a load that takes a moment.

## The look: the N9's theme

Through `image://theme/`: the tool bar's icons, the stop and clear icons
(`icon-m-browser-stop`, `-clear`), the components' own. These are Nokia's
(the "blanco" theme of one's own N9 or N950) and are not in this
repository.

## Building

Qt 5 (Quick) and qt-components' `com.nokia.meego` 2.0 at run time, and
for pages the QML module of Qt WebEngine (Debian/Devuan:
`qml-module-qtwebengine`; FreeBSD: `www/qt5-webengine`) or of Qt WebKit
(`qml-module-qtwebkit`; `www/qt5-webkit`); neither is needed to build:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
