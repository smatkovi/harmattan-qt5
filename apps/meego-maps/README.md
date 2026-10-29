# meego-maps

The Nokia N9's maps, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`), for the MeeGo session on Maemo
Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's Maps is closed
source; this app imitates its look and behaviour, it contains none of
Nokia's code or files (nor Nemo's: the code is new).

- **The map** full screen: drag to move, pinch, the mouse wheel, a double
  tap or the zoom keys at the side to zoom; `+`, `-` and the arrow keys
  on a keyboard. Where it was is there again at the next start.
- **One's position**, only when asked (the tool bar's position key): a
  blue dot with its accuracy around it, the map following it until it is
  moved; "Position not available" without a source.
- **A place**: found by name (the tool bar's search, the places near the
  map's middle first, with their distance) or held on the map (a long
  press), with its address in a bubble above its pin.
- **Online or offline**: OpenStreetMap's tiles and Nominatim's search and
  addresses online; where OSM Scout Server answers on the device
  (`http://127.0.0.1:8553`), its tiles and its search offline (the menu
  switches between them; offline a held point shows its coordinates,
  OSM Scout Server has no reverse lookup).
- `meego-maps geo:47.56,13.65` (or `geo:…?q=Label`) shows that place.

The map is drawn by the app itself, raster tiles in OpenStreetMap's
z/x/y scheme in QML (no Qt Location needed), the level before staying
under the new one while it loads. The tiles come with the app's own
User-Agent, as the tile servers ask, and are kept on disk
(`~/.cache/meego/meego-maps/tiles`, up to 200 MB); "© OpenStreetMap
contributors" stays in the corner. The position is Qt Positioning's
(GeoClue 2, or whatever plugin is installed) and is only asked for after
the key is pressed. `MEEGO_MAPS_TILES` gives another tile server
(`…/%z/%x/%y.png`), `MEEGO_MAPS_NOMINATIM` another Nominatim,
`MEEGO_MAPS_OSMSCOUT` another OSM Scout Server.

`tests/fake-osmscout.py` stands in for OSM Scout Server in tests (tiles
with their numbers, a made-up search; needs Pillow).

## The look: the N9's theme

Through `image://theme/`: the pins (`icon-m-common-location-selected`),
the position key, the tool bar's icons, the components' own. These are
Nokia's (the "blanco" theme of one's own N9 or N950) and are not in this
repository.

## Building

Qt 5 (Quick, Network) and qt-components' `com.nokia.meego` 2.0 at run
time; for the position Qt Positioning's QML module (Debian/Devuan:
`qml-module-qtpositioning`; FreeBSD: `devel/qt5-location`), without it
the rest works:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
