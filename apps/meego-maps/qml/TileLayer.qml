import QtQuick 2.0

// The tiles of one zoom level over the map's view, scaled around the
// view's middle to the map's zoom between the levels.
Item {
    id: tileLayer

    property Item map
    property int level: 0
    property string url
    readonly property real world: 256 * Math.pow(2, level)
    readonly property real cx: map.mx * world
    readonly property real cy: map.my * world
    readonly property int loading: _loading
    property int _loading: 0

    width: map.width
    height: map.height
    scale: Math.pow(2, map.zoom - level)
    transformOrigin: Item.Center

    ListModel { id: tiles }
    onUrlChanged: { tiles.clear(); Qt.callLater(update) }
    onLevelChanged: Qt.callLater(update)

    // the tiles the view needs now: the ones gone go, the new ones come
    function update() {
        if (url === "" || width <= 0) {
            tiles.clear()
            return
        }
        var n = Math.pow(2, level), hw = map.width / 2 / scale, hh = map.height / 2 / scale
        var x0 = Math.floor((cx - hw) / 256), x1 = Math.floor((cx + hw) / 256)
        var y0 = Math.max(0, Math.floor((cy - hh) / 256)), y1 = Math.min(n - 1, Math.floor((cy + hh) / 256))
        if (x1 - x0 > 16 || y1 - y0 > 16)
            return
        var need = {}
        for (var x = x0; x <= x1; ++x)
            for (var y = y0; y <= y1; ++y)
                need[level + "/" + x + "/" + y] = { x: x, y: y }
        for (var i = tiles.count - 1; i >= 0; --i) {
            var k = tiles.get(i).key
            if (need[k] === undefined || tiles.get(i).url !== tileLayer.tileUrl(tiles.get(i).tx, tiles.get(i).ty))
                tiles.remove(i)
            else
                delete need[k]
        }
        for (var key in need)
            tiles.append({ key: key, tx: need[key].x, ty: need[key].y, url: tileUrl(need[key].x, need[key].y) })
    }
    function tileUrl(x, y) {
        var n = Math.pow(2, level)
        return url.replace("%z", level).replace("%x", ((x % n) + n) % n).replace("%y", y)
    }
    function clear() { tiles.clear() }

    Repeater {
        model: tiles
        Image {
            x: model.tx * 256 - tileLayer.cx + tileLayer.width / 2
            y: model.ty * 256 - tileLayer.cy + tileLayer.height / 2
            width: 256; height: 256
            source: model.url
            asynchronous: true
            smooth: true
            fillMode: Image.Stretch
            opacity: status === Image.Ready ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 150 } }
        }
    }
}
