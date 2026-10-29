import QtQuick 2.0
import com.nokia.meego 2.0

// Places by name, near the map's middle first: OpenStreetMap's Nominatim,
// or OSM Scout Server's own search offline; a tap shows one on the map.
Page {
    id: page

    property real lat
    property real lon
    property var list: []
    property string error: ""
    signal picked(var place)

    function distance(a, b, c, d) {
        var r = Math.PI / 180, x = (d - b) * r * Math.cos((a + c) / 2 * r), y = (c - a) * r
        var km = Math.sqrt(x * x + y * y) * 6371
        return km < 1 ? Math.round(km * 1000) + " m" : km < 10 ? km.toFixed(1) + " km" : Math.round(km) + " km"
    }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
    }

    Connections {
        target: places
        onResults: { page.list = places; page.error = error }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    TextField {
        id: field
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: 12 }
        placeholderText: places.offline ? qsTr("Search (offline)") : qsTr("Search")
        inputMethodHints: Qt.ImhNoPredictiveText
        Keys.onReturnPressed: go()
        Keys.onEnterPressed: go()
        function go() {
            if (text.trim() === "")
                return
            page.error = ""
            page.list = []
            places.search(text, page.lat, page.lon)
        }
        Component.onCompleted: forceActiveFocus()
    }

    ListView {
        id: view
        anchors { top: field.bottom; topMargin: 8; left: parent.left; right: parent.right; bottom: parent.bottom }
        clip: true
        model: page.list
        delegate: Item {
            width: view.width
            height: 88
            Rectangle { anchors.fill: parent; color: area.pressed ? "#d9d9d9" : "transparent" }
            Image {
                id: pin
                anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
                source: "image://theme/icon-m-common-location-selected"
            }
            Column {
                anchors { left: pin.right; leftMargin: 12; right: dist.left; rightMargin: 12; verticalCenter: parent.verticalCenter }
                Label {
                    width: parent.width
                    text: modelData.title
                    font.pixelSize: 26
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#282828"
                }
                Label {
                    width: parent.width
                    visible: text !== ""
                    text: modelData.detail
                    font.pixelSize: 20
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#8c8c8c"
                }
            }
            Label {
                id: dist
                anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
                text: page.distance(page.lat, page.lon, modelData.lat, modelData.lon)
                font.pixelSize: 20
                color: "#8c8c8c"
            }
            Rectangle {
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
                height: 1
                color: "#d2d2d2"
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: { page.picked(modelData); pageStack.pop() }
            }
        }
    }
    ScrollDecorator { flickableItem: view }

    BusyIndicator {
        anchors.centerIn: view
        visible: places.busy
        running: visible
        platformStyle: BusyIndicatorStyle { size: "large" }
    }
    Label {
        anchors.centerIn: view
        visible: !places.busy && page.error !== ""
        width: parent.width - 64
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        text: page.error
        font.pixelSize: 32
        color: "#b4b4b4"
    }
}
