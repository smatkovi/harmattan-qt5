import QtQuick 2.0

// A map of raster tiles (OpenStreetMap's scheme, z/x/y, 256 pixels): drag
// to move, pinch, the wheel or a double tap to zoom; a long press says
// where. The middle is kept in the world's unit square (mx, my); the
// level below the zoom is drawn, the one before stays under it until the
// new tiles are there. Children are drawn over the map: place them with
// screenX(lon) and screenY(lat).
Item {
    id: map

    property real mx: 0.5
    property real my: 0.5
    property real zoom: 3
    property real minZoom: 2
    property real maxZoom: 19
    property string tileUrl
    readonly property real lat: yToLat(my)
    readonly property real lon: xToLon(mx)
    readonly property bool moving: drag.pressed || pinch.pinch.active
    default property alias overlay: overlay.data
    signal held(real lat, real lon)
    signal tapped(real x, real y)

    clip: true

    function latToY(lat) {
        var r = lat * Math.PI / 180
        return (1 - Math.log(Math.tan(r) + 1 / Math.cos(r)) / Math.PI) / 2
    }
    function yToLat(y) {
        var n = Math.PI - 2 * Math.PI * y
        return 180 / Math.PI * Math.atan(0.5 * (Math.exp(n) - Math.exp(-n)))
    }
    function lonToX(lon) { return (lon + 180) / 360 }
    function xToLon(x) { return x * 360 - 180 }
    function wrap(x) { return x - Math.floor(x) }
    function clampY(y) { return Math.max(0, Math.min(1, y)) }
    function clampZoom(z) { return Math.max(minZoom, Math.min(maxZoom, z)) }
    function scaleAt(z) { return 256 * Math.pow(2, z) }

    function center(lat, lon, z) {
        mx = wrap(lonToX(lon))
        my = clampY(latToY(lat))
        if (z !== undefined)
            zoom = clampZoom(z)
    }
    function screenX(lon) {
        var d = lonToX(lon) - mx
        if (d > 0.5) d -= 1
        if (d < -0.5) d += 1
        return width / 2 + d * scaleAt(zoom)
    }
    function screenY(lat) { return height / 2 + (latToY(lat) - my) * scaleAt(zoom) }
    function coordinateAt(x, y) {
        var s = scaleAt(zoom)
        return { lat: yToLat(clampY(my + (y - height / 2) / s)), lon: xToLon(wrap(mx + (x - width / 2) / s)) }
    }
    function pan(dx, dy) {
        var s = scaleAt(zoom)
        mx = wrap(mx - dx / s)
        my = clampY(my - dy / s)
    }
    // to zoom z with the point at (x, y) staying where it is
    function zoomAt(z, x, y) {
        z = clampZoom(z)
        var s0 = scaleAt(zoom), s1 = scaleAt(z)
        var wx = mx + (x - width / 2) / s0, wy = my + (y - height / 2) / s0
        zoom = z
        mx = wrap(wx - (x - width / 2) / s1)
        my = clampY(wy - (y - height / 2) / s1)
    }
    function zoomBy(d) { zoomAnimation.to = clampZoom(Math.round(zoom + d)); zoomAnimation.restart() }

    NumberAnimation { id: zoomAnimation; target: map; property: "zoom"; duration: 200; easing.type: Easing.OutQuad }

    // OpenStreetMap's land, while the tiles come
    Rectangle { anchors.fill: parent; color: "#f2efe9" }

    readonly property int level: Math.floor(Math.max(minZoom, Math.min(maxZoom, zoom)) + 1e-6)
    TileLayer { id: back; map: map; url: map.tileUrl; visible: backTimer.running }
    TileLayer { id: front; map: map; url: map.tileUrl }
    Component.onCompleted: { front.level = level; refresh() }
    onLevelChanged: {
        // the old level stays under the new for a moment
        back.level = front.level
        front.level = level
        backTimer.restart()
        refresh()
    }
    Timer { id: backTimer; interval: 1500; onTriggered: back.clear() }

    function refresh() { Qt.callLater(front.update); if (backTimer.running) Qt.callLater(back.update) }
    onMxChanged: refresh()
    onMyChanged: refresh()
    onZoomChanged: refresh()
    onWidthChanged: refresh()
    onHeightChanged: refresh()

    PinchArea {
        id: pinch
        anchors.fill: parent
        property real startZoom
        onPinchStarted: startZoom = map.zoom
        onPinchUpdated: {
            map.pan(pinch.center.x - pinch.previousCenter.x, pinch.center.y - pinch.previousCenter.y)
            map.zoomAt(startZoom + Math.log(pinch.scale) / Math.LN2, pinch.center.x, pinch.center.y)
        }
        onPinchFinished: map.zoomBy(0)
        MouseArea {
            id: drag
            anchors.fill: parent
            property real lastX
            property real lastY
            property bool moved: false
            onPressed: { lastX = mouse.x; lastY = mouse.y; moved = false }
            onPositionChanged: {
                if (!moved && Math.abs(mouse.x - lastX) + Math.abs(mouse.y - lastY) < 8)
                    return
                moved = true
                map.pan(mouse.x - lastX, mouse.y - lastY)
                lastX = mouse.x
                lastY = mouse.y
            }
            onClicked: if (!moved) map.tapped(mouse.x, mouse.y)
            onDoubleClicked: {
                zoomAnimation.stop()
                var target = map.clampZoom(Math.round(map.zoom) + 1), x = mouse.x, y = mouse.y
                map.zoomAt(target, x, y)
            }
            onPressAndHold: if (!moved) { var c = map.coordinateAt(mouse.x, mouse.y); map.held(c.lat, c.lon) }
            onWheel: map.zoomAt(map.zoom + wheel.angleDelta.y / 480, wheel.x, wheel.y)
        }
    }

    Item { id: overlay; anchors.fill: parent }

    // the tiles' licence, as OpenStreetMap asks
    Rectangle {
        anchors { right: parent.right; bottom: parent.bottom }
        width: credit.width + 12
        height: credit.height + 4
        color: "#b3ffffff"
        Text {
            id: credit
            anchors.centerIn: parent
            text: "© OpenStreetMap contributors"
            font.pixelSize: 14
            color: "#333333"
        }
    }
}
