import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// A list of songs (all, an album's, an artist's, a genre's, a playlist):
// a tap plays from there.
Page {
    id: page

    property string title
    property string subtitle
    property string cover
    property var songs: []
    property bool numbered: false

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon {
            iconId: "toolbar-shuffle"
            enabled: page.songs.length > 0
            onClicked: {
                player.shuffle = true
                player.playQueue(page.songs, Math.floor(Math.random() * page.songs.length))
                window.nowPlaying()
            }
        }
        ToolIcon {
            iconId: player.playing ? "toolbar-mediacontrol-pause" : "toolbar-mediacontrol-play"
            visible: player.active
            onClicked: window.nowPlaying()
        }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    ListView {
        id: list
        anchors.fill: parent
        model: page.songs
        header: Item {
            width: list.width
            height: page.cover !== "" ? 168 : 80
            Cover {
                id: headCover
                visible: page.cover !== ""
                x: 16; y: 16
                width: 136; height: 136
                source: page.cover
            }
            Column {
                anchors { left: headCover.visible ? headCover.right : parent.left; leftMargin: 16
                          right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
                Label {
                    width: parent.width
                    text: page.title
                    font.pixelSize: 32
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "white"
                }
                Label {
                    width: parent.width
                    text: page.subtitle !== "" ? page.subtitle
                          : Util.count(page.songs.length, qsTr("1 song"), qsTr("%1 songs"))
                    font.pixelSize: 22
                    color: "#8c8c8c"
                }
            }
        }
        delegate: ListRow {
            title: (page.numbered && modelData.track > 0 ? modelData.track + ". " : "") + modelData.title
            subtitle: page.numbered ? "" : Util.artistOf(modelData) + "  ·  " + Util.albumOf(modelData)
            value: Util.time(modelData.duration)
            highlighted: player.current.url === modelData.url
            onClicked: {
                player.shuffle = false
                player.playQueue(page.songs, index)
                window.nowPlaying()
            }
        }
    }
    ScrollDecorator { flickableItem: list }
}
