import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The albums (all, or an artist's) as covers with their names.
Page {
    id: page

    property string artist
    property var albums: library.albums(artist)

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon {
            iconId: "toolbar-shuffle"
            visible: page.artist !== ""
            onClicked: {
                var songs = library.artistSongs(page.artist)
                if (songs.length > 0) {
                    player.shuffle = true
                    player.playQueue(songs, Math.floor(Math.random() * songs.length))
                    window.nowPlaying()
                }
            }
        }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    GridView {
        id: grid
        anchors { fill: parent; leftMargin: 8 }
        readonly property int columns: Math.max(2, Math.floor(width / 200))
        cellWidth: width / columns
        cellHeight: cellWidth + 64
        model: page.albums
        header: PageTitle { text: page.artist !== "" ? page.artist : qsTr("Albums") }
        delegate: Item {
            width: grid.cellWidth
            height: grid.cellHeight
            Cover {
                id: cover
                x: 8; y: 8
                width: parent.width - 16
                height: width
                source: modelData.cover
                opacity: area.pressed ? 0.6 : 1
            }
            Label {
                anchors { left: cover.left; right: cover.right; top: cover.bottom; topMargin: 4 }
                text: modelData.name
                font.pixelSize: 22
                font.bold: true
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                color: "white"
            }
            Label {
                anchors { left: cover.left; right: cover.right; top: cover.bottom; topMargin: 30 }
                text: modelData.artist
                font.pixelSize: 20
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                color: "#8c8c8c"
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: pageStack.push(Qt.resolvedUrl("SongsPage.qml"), {
                    title: modelData.name, subtitle: modelData.artist, cover: modelData.cover,
                    numbered: true, songs: library.albumSongs(modelData.name, modelData.artist) })
            }
        }
    }
    ScrollDecorator { flickableItem: grid }
}
