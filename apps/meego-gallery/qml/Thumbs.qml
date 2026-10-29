import QtQuick 2.0

// Pictures as square thumbnails without gaps between them, as the N9's
// gallery shows them; a tap opens one full screen.
GridView {
    id: grid

    property var items: []
    signal picked(int index)

    readonly property int columns: Math.max(3, Math.round(width / 160))
    cellWidth: width / columns
    cellHeight: cellWidth
    model: items
    clip: true
    cacheBuffer: cellHeight * 4

    delegate: Item {
        width: grid.cellWidth
        height: grid.cellHeight
        Rectangle { anchors { fill: parent; margins: 1 } color: "#262626" }
        Image {
            anchors { fill: parent; margins: 1 }
            source: modelData.url
            sourceSize.width: grid.cellWidth * 1.5
            fillMode: Image.PreserveAspectCrop
            asynchronous: true
            clip: true
            smooth: true
            opacity: area.pressed ? 0.6 : 1
        }
        MouseArea {
            id: area
            anchors.fill: parent
            onClicked: grid.picked(index)
        }
    }
}
