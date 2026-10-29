import QtQuick 2.0
import com.nokia.meego 2.0

// The videos, the newest first: each with its picture, its length on the
// N9's duration plate and its title; a tap plays it, a long press deletes.
Page {
    id: page

    function time(ms) {
        var s = Math.floor(ms / 1000), m = Math.floor(s / 60), h = Math.floor(m / 60)
        var ss = (s % 60 < 10 ? "0" : "") + (s % 60)
        return h > 0 ? h + ":" + (m % 60 < 10 ? "0" : "") + (m % 60) + ":" + ss : m + ":" + ss
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-refresh"
            enabled: !library.scanning
            opacity: enabled ? 1 : 0.4
            onClicked: library.rescan()
        }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    GridView {
        id: grid
        anchors { fill: parent; margins: 8 }
        readonly property int columns: Math.max(2, Math.floor(width / 260))
        cellWidth: width / columns
        cellHeight: cellWidth * 9 / 16 + 56
        model: library.videos
        clip: true
        delegate: Item {
            width: grid.cellWidth
            height: grid.cellHeight
            Item {
                id: picture
                x: 6; y: 6
                width: parent.width - 12
                height: width * 9 / 16
                BorderImage {
                    anchors.fill: parent
                    source: "image://theme/meegotouch-video-placeholder"
                    visible: thumb.status !== Image.Ready
                }
                Rectangle { anchors.fill: parent; color: "#262626"; visible: thumb.status !== Image.Ready; z: -1 }
                Image {
                    id: thumb
                    anchors.fill: parent
                    source: modelData.thumbnail
                    fillMode: Image.PreserveAspectCrop
                    clip: true
                    asynchronous: true
                    opacity: area.pressed ? 0.6 : 1
                }
                BorderImage {
                    anchors { right: parent.right; bottom: parent.bottom; margins: 6 }
                    visible: modelData.duration > 0
                    width: length.width + 16
                    height: length.height + 6
                    source: "image://theme/meegotouch-video-duration-background"
                    border { left: 6; right: 6; top: 6; bottom: 6 }
                    Rectangle { anchors.fill: parent; color: "#99000000"; radius: 4; z: -1 }
                    Label {
                        id: length
                        anchors.centerIn: parent
                        text: page.time(modelData.duration)
                        font.pixelSize: 18
                        color: "white"
                    }
                }
            }
            Label {
                anchors { left: picture.left; right: picture.right; top: picture.bottom; topMargin: 6 }
                text: modelData.title
                font.pixelSize: 22
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                color: "white"
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: window.play(modelData.url, modelData.title)
                onPressAndHold: { confirm.path = modelData.path; confirm.message = modelData.title; confirm.open() }
            }
        }
    }

    Label {
        anchors.centerIn: parent
        visible: library.videos.length === 0
        text: library.scanning ? qsTr("Looking for videos…") : qsTr("No videos")
        font.pixelSize: 32
        color: "#8c8c8c"
    }

    QueryDialog {
        id: confirm
        property string path
        titleText: qsTr("Delete video?")
        acceptButtonText: qsTr("Delete")
        rejectButtonText: qsTr("Cancel")
        onAccepted: library.remove(path)
    }
}
