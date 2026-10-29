import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// Now playing, as on the N9: the cover large, the song, its artist and
// album, where it is (to drag), shuffle and repeat; previous, play or
// pause and next in the tool bar.
Page {
    id: page
    objectName: "nowPlaying"

    readonly property bool landscape: width > height

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon { iconId: "toolbar-mediacontrol-previous"; onClicked: player.previous() }
        ToolIcon {
            iconId: player.playing ? "toolbar-mediacontrol-pause" : "toolbar-mediacontrol-play"
            onClicked: player.toggle()
        }
        ToolIcon {
            iconId: "toolbar-mediacontrol-next"
            enabled: player.index + 1 < player.count || player.repeat
            opacity: enabled ? 1 : 0.4
            onClicked: player.next()
        }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    Cover {
        id: cover
        x: page.landscape ? 0 : (page.width - width) / 2
        y: page.landscape ? 0 : 24
        width: page.landscape ? page.height : Math.min(page.width - 48, page.height * 0.55)
        height: width
        source: player.current.cover || ""
    }

    Item {
        id: info
        anchors { left: page.landscape ? cover.right : parent.left
                  top: page.landscape ? parent.top : cover.bottom
                  right: parent.right; bottom: parent.bottom; margins: 24 }

        Column {
            id: texts
            width: parent.width
            spacing: 4
            Label {
                width: parent.width
                text: player.current.title || ""
                font.pixelSize: 32
                font.bold: true
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                color: "white"
            }
            Label {
                width: parent.width
                text: player.active ? Util.artistOf(player.current) : ""
                font.pixelSize: 26
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                color: "#c8c8c8"
            }
            Label {
                width: parent.width
                text: player.active ? Util.albumOf(player.current) : ""
                font.pixelSize: 22
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                color: "#8c8c8c"
            }
            Label {
                width: parent.width
                visible: player.count > 1
                text: qsTr("%1 of %2").arg(player.index + 1).arg(player.count)
                font.pixelSize: 20
                color: "#8c8c8c"
            }
        }

        Column {
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
            spacing: 8
            Row {
                anchors.right: parent.right
                spacing: 24
                Image {
                    source: "image://theme/icon-m-music-video-shuffle-on"
                    opacity: player.shuffle ? 1 : 0.35
                    MouseArea { anchors.fill: parent; anchors.margins: -12; onClicked: player.shuffle = !player.shuffle }
                }
                Image {
                    source: "image://theme/icon-m-music-video-repeat-on"
                    opacity: player.repeat ? 1 : 0.35
                    MouseArea { anchors.fill: parent; anchors.margins: -12; onClicked: player.repeat = !player.repeat }
                }
            }
            Slider {
                id: slider
                width: parent.width
                minimumValue: 0
                maximumValue: Math.max(1, player.duration)
                stepSize: 1000
                valueIndicatorVisible: true
                valueIndicatorText: Util.time(value)
                // follows the song, unless being dragged
                Binding on value { when: !slider.pressed; value: player.position }
                onPressedChanged: if (!pressed) player.seek(value)
            }
            Item {
                width: parent.width
                height: elapsed.height
                Label {
                    id: elapsed
                    text: Util.time(player.position)
                    font.pixelSize: 20
                    color: "#8c8c8c"
                }
                Label {
                    anchors.right: parent.right
                    text: Util.time(player.duration)
                    font.pixelSize: 20
                    color: "#8c8c8c"
                }
            }
        }
    }
}
