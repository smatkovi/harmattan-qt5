import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The music player's main view, as on the N9: the covers of the music
// played lately as a mosaic, with the song that plays over it, and the
// library's views beside it (landscape) or below it (portrait).
Page {
    id: page

    property var covers: library.mosaic(9)
    readonly property bool landscape: width > height

    Connections { target: library; onChanged: page.covers = library.mosaic(9) }
    Connections { target: player; onCurrentChanged: page.covers = library.mosaic(9) }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-refresh"
            enabled: !library.scanning
            opacity: enabled ? 1 : 0.4
            onClicked: library.rescan()
        }
        ToolIcon {
            iconId: "toolbar-shuffle"
            onClicked: {
                player.shuffle = true
                var all = library.songs()
                if (all.length > 0) {
                    player.playQueue(all, Math.floor(Math.random() * all.length))
                    window.nowPlaying()
                }
            }
        }
        ToolIcon {
            iconId: player.playing ? "toolbar-mediacontrol-pause" : "toolbar-mediacontrol-play"
            visible: player.active
            onClicked: window.nowPlaying()
        }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    Item {
        id: mosaic
        width: page.landscape ? page.height : page.width
        height: width
        Grid {
            columns: 3
            Repeater {
                model: 9
                Cover {
                    width: mosaic.width / 3
                    height: mosaic.height / 3
                    source: index < page.covers.length ? page.covers[index] : ""
                }
            }
        }
        // the lines between the tiles
        Repeater {
            model: 2
            Rectangle { x: (index + 1) * mosaic.width / 3 - 1; width: 2; height: mosaic.height; color: "black" }
        }
        Repeater {
            model: 2
            Rectangle { y: (index + 1) * mosaic.height / 3 - 1; height: 2; width: mosaic.width; color: "black" }
        }
        // the song that plays
        Rectangle {
            visible: player.active
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
            height: 88
            color: "#cc000000"
            Column {
                anchors { left: parent.left; right: parent.right; margins: 16; verticalCenter: parent.verticalCenter }
                Label {
                    width: parent.width
                    text: qsTr("Now playing")
                    font.pixelSize: 20
                    color: "#ef8b1e"
                }
                Label {
                    width: parent.width
                    text: (player.current.title || "") + "  –  " + Util.artistOf(player.current)
                    font.pixelSize: 24
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "white"
                }
            }
            MouseArea { anchors.fill: parent; onClicked: window.nowPlaying() }
        }
        Label {
            anchors.centerIn: parent
            visible: library.count === 0
            width: parent.width - 64
            horizontalAlignment: Text.AlignHCenter
            text: library.scanning ? qsTr("Looking for music…")
                                   : qsTr("No music in %1").arg(library.folder)
            font.pixelSize: 26
            color: "#8c8c8c"
            wrapMode: Text.Wrap
        }
    }

    ListView {
        id: views
        anchors { left: page.landscape ? mosaic.right : parent.left
                  top: page.landscape ? parent.top : mosaic.bottom
                  right: parent.right; bottom: parent.bottom }
        clip: true
        model: ListModel {
            ListElement { key: "songs"; name: "All songs"; icon: "icon-m-music-video-all-songs" }
            ListElement { key: "artists"; name: "Artists"; icon: "icon-m-content-artist-inverse" }
            ListElement { key: "albums"; name: "Albums"; icon: "icon-m-content-album-inverse" }
            ListElement { key: "genres"; name: "Genres"; icon: "icon-m-content-genre-inverse" }
            ListElement { key: "playlists"; name: "Playlists"; icon: "icon-m-content-playlist-inverse" }
        }
        delegate: ListRow {
            icon: model.icon
            title: qsTr(model.name)
            value: {
                library.count  // follows the library
                switch (model.key) {
                case "songs": return library.count
                case "artists": return library.artists().length
                case "albums": return library.albums("").length
                case "genres": return library.genres().length
                }
                return ""
            }
            drilldown: true
            onClicked: {
                switch (model.key) {
                case "songs":
                    pageStack.push(Qt.resolvedUrl("SongsPage.qml"), { title: qsTr("All songs"), songs: library.songs() })
                    break
                case "artists":
                    pageStack.push(Qt.resolvedUrl("ArtistsPage.qml"))
                    break
                case "albums":
                    pageStack.push(Qt.resolvedUrl("AlbumsPage.qml"), { artist: "" })
                    break
                case "genres":
                    pageStack.push(Qt.resolvedUrl("GenresPage.qml"))
                    break
                case "playlists":
                    pageStack.push(Qt.resolvedUrl("PlaylistsPage.qml"))
                    break
                }
            }
        }
    }
    BusyIndicator {
        anchors { right: parent.right; bottom: parent.bottom; margins: 16 }
        visible: library.scanning
        running: visible
        platformStyle: BusyIndicatorStyle { size: "small" }
    }
}
