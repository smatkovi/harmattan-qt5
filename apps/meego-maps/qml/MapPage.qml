import QtQuick 2.0
import com.nokia.meego 2.0

// The map full screen: one's position (only when asked, with the tool
// bar's position key: then the map follows it until it is moved), a place
// found by name or held on the map with its address in a bubble, zoom
// keys at the side; the map is OpenStreetMap's online or OSM Scout
// Server's on the device. Where it was is kept for the next start.
Page {
    id: page

    property bool locating: false
    property bool following: false
    property var pin: null          // {lat, lon, title, detail}
    property bool bubble: false
    property QtObject gps: null       // Position.qml, where Qt Positioning is
    readonly property bool hasFix: gps !== null && gps.position.latitudeValid && gps.position.longitudeValid

    function showPlace(p, zoom) {
        pin = { lat: p.lat, lon: p.lon, title: p.title || "", detail: p.detail || "" }
        bubble = true
        following = false
        map.center(p.lat, p.lon, zoom !== undefined ? zoom : Math.max(map.zoom, 15))
    }
    function locate() {
        if (locating) {
            locating = following = false
            return
        }
        if (!gps || !gps.valid) {
            banner.text = qsTr("Position not available")
            banner.show()
            return
        }
        locating = following = true
        noFix.restart()
        if (hasFix)
            map.center(gps.position.coordinate.latitude, gps.position.coordinate.longitude, Math.max(map.zoom, 15))
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconSource: "image://theme/icon-m-common-location" + (page.locating ? "-selected" : "")
            onClicked: page.locate()
        }
        ToolIcon {
            iconId: "toolbar-search"
            onClicked: {
                var s = pageStack.push(Qt.resolvedUrl("SearchPage.qml"), { lat: map.lat, lon: map.lon })
                s.picked.connect(function (p) { page.showPlace(p) })
            }
        }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: menu.open() }
    }

    Menu {
        id: menu
        MenuLayout {
            MenuItem {
                visible: places.offlineAvailable
                text: places.offline ? qsTr("Online map") : qsTr("Offline map")
                onClicked: places.offline = !places.offline
            }
            MenuItem {
                visible: page.pin !== null
                text: qsTr("Remove the place")
                onClicked: { page.pin = null; page.bubble = false }
            }
        }
    }

    function keep() {
        stateStore.setValue("lat", map.lat)
        stateStore.setValue("lon", map.lon)
        stateStore.setValue("zoom", map.zoom)
        if (places.offlineAvailable)
            stateStore.setValue("offline", places.offline)
    }
    Component.onCompleted: {
        var c = Qt.createComponent(Qt.resolvedUrl("Position.qml"))
        if (c.status === Component.Ready)
            gps = c.createObject(page)
        else
            console.log("meego-maps: no position: " + c.errorString())
        places.offline = stateStore.value("offline", true) === true || stateStore.value("offline", true) === "true"
        map.center(Number(stateStore.value("lat", 48.2085)), Number(stateStore.value("lon", 16.3721)), Number(stateStore.value("zoom", 12)))
        if (startPlace.lat !== undefined)
            showPlace({ lat: startPlace.lat, lon: startPlace.lon, title: startPlace.label || "", detail: "" }, 15)
        if (startPlace.lat !== undefined && !startPlace.label)
            places.reverse(startPlace.lat, startPlace.lon)
    }
    Component.onDestruction: keep()
    Timer {
        // kept now and then too: the application may end without notice
        interval: 5000; running: true; repeat: true
        onTriggered: page.keep()
    }

    Binding { target: page.gps; property: "active"; value: page.locating; when: page.gps !== null }
    Connections {
        target: page.gps
        ignoreUnknownSignals: true
        onFailed: {
            page.locating = page.following = false
            noFix.stop()
            banner.text = qsTr("Position not available")
            banner.show()
        }
        onPositionChanged: {
            if (!page.hasFix)
                return
            noFix.stop()
            if (page.following && !map.moving)
                map.center(page.gps.position.coordinate.latitude, page.gps.position.coordinate.longitude)
        }
    }
    Timer {
        id: noFix
        interval: 30000
        onTriggered: { banner.text = qsTr("No position yet"); banner.show() }
    }

    Connections {
        target: places
        onAddress: {
            if (page.pin && Math.abs(page.pin.lat - lat) < 1e-9 && Math.abs(page.pin.lon - lon) < 1e-9)
                page.pin = { lat: lat, lon: lon, title: title, detail: detail }
        }
    }

    MapView {
        id: map
        anchors.fill: parent
        tileUrl: tileOverride !== "" ? tileOverride
                 : places.offline ? osmScout + "/v1/tile?style=default&daylight=1&shift=0&scale=1&z=%z&x=%x&y=%y"
                                  : "https://tile.openstreetmap.org/%z/%x/%y.png"
        onMovingChanged: if (moving) page.following = false
        onHeld: {
            page.pin = { lat: lat, lon: lon, title: qsTr("Looking up the address…"), detail: "" }
            page.bubble = true
            places.reverse(lat, lon)
        }
        onTapped: page.bubble = false

        // one's position: the accuracy around a blue dot
        Item {
            visible: page.locating && page.hasFix
            x: page.hasFix ? map.screenX(page.gps.position.coordinate.longitude) : 0
            y: page.hasFix ? map.screenY(page.gps.position.coordinate.latitude) : 0
            Rectangle {
                readonly property real metres: page.hasFix && page.gps.position.horizontalAccuracyValid ? page.gps.position.horizontalAccuracy : 0
                // metres per pixel at the latitude
                readonly property real r: page.hasFix ? metres / (156543.03 * Math.cos(page.gps.position.coordinate.latitude * Math.PI / 180) / Math.pow(2, map.zoom)) : 0
                width: 2 * r; height: width; radius: r
                x: -r; y: -r
                color: "#303591e7"
                border { color: "#803591e7"; width: 1 }
                visible: r > 12
            }
            Rectangle {
                width: 22; height: 22; radius: 11
                x: -11; y: -11
                color: "#3591e7"
                border { color: "white"; width: 3 }
            }
        }

        // the place, with its bubble above it
        Item {
            visible: page.pin !== null
            x: page.pin ? map.screenX(page.pin.lon) : 0
            y: page.pin ? map.screenY(page.pin.lat) : 0
            Image {
                id: pinImage
                x: -width / 2; y: -height + 4
                source: "image://theme/icon-m-common-location-selected"
                MouseArea { anchors { fill: parent; margins: -8 } onClicked: page.bubble = !page.bubble }
            }
            Rectangle {
                id: bubbleBox
                visible: page.bubble
                width: Math.min(map.width - 32, Math.max(titleLabel.implicitWidth, detailLabel.implicitWidth) + 32)
                height: column.height + 20
                x: Math.max(16 - parent.x, Math.min(map.width - 16 - width - parent.x, -width / 2))
                y: pinImage.y - height - 6
                radius: 8
                color: "white"
                border { color: "#c8c8c8"; width: 1 }
                Column {
                    id: column
                    x: 16; y: 10
                    width: parent.width - 32
                    Label {
                        id: titleLabel
                        width: parent.width
                        text: page.pin ? page.pin.title : ""
                        font.pixelSize: 24
                        font.bold: true
                        elide: Text.ElideRight
                        wrapMode: Text.NoWrap
                        color: "#282828"
                    }
                    Label {
                        id: detailLabel
                        width: parent.width
                        visible: text !== ""
                        text: page.pin ? page.pin.detail : ""
                        font.pixelSize: 20
                        maximumLineCount: 2
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: "#6e6e6e"
                    }
                }
            }
        }
    }

    // zoom keys
    Column {
        anchors { right: parent.right; rightMargin: 12; verticalCenter: parent.verticalCenter }
        spacing: 8
        Repeater {
            model: ["+", "−"]
            Rectangle {
                width: 56; height: 56; radius: 8
                color: zoomArea.pressed ? "#d9d9d9" : "#e6ffffff"
                border { color: "#b4b4b4"; width: 1 }
                Label {
                    anchors.centerIn: parent
                    text: modelData
                    font.pixelSize: 36
                    color: "#282828"
                }
                MouseArea {
                    id: zoomArea
                    anchors.fill: parent
                    onClicked: map.zoomBy(index === 0 ? 1 : -1)
                }
            }
        }
    }

    focus: true
    onStatusChanged: if (status === PageStatus.Active) forceActiveFocus()
    Keys.onPressed: {
        if (event.text === "+" || event.text === "=") map.zoomBy(1)
        else if (event.text === "-") map.zoomBy(-1)
        else if (event.key === Qt.Key_Left) map.pan(100, 0)
        else if (event.key === Qt.Key_Right) map.pan(-100, 0)
        else if (event.key === Qt.Key_Up) map.pan(0, 100)
        else if (event.key === Qt.Key_Down) map.pan(0, -100)
        else return
        event.accepted = true
    }
}
