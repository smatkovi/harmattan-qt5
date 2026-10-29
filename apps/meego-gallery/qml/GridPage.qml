import QtQuick 2.0
import com.nokia.meego 2.0

// The gallery's main view: all pictures, the newest first, or the albums
// (the folders, each with its newest picture and its count).
Page {
    id: page

    property bool showAlbums: false

    tools: ToolBarLayout {
        ButtonRow {
            platformStyle: TabButtonStyle {}
            TabButton {
                iconSource: "image://theme/icon-m-toolbar-grid" + (theme.inverted ? "-white" : "")
                checked: !page.showAlbums
                onClicked: page.showAlbums = false
            }
            TabButton {
                iconSource: "image://theme/icon-m-toolbar-gallery" + (theme.inverted ? "-white" : "")
                checked: page.showAlbums
                onClicked: page.showAlbums = true
            }
        }
        ToolIcon {
            iconId: "toolbar-refresh"
            enabled: !pictures.scanning
            opacity: enabled ? 1 : 0.4
            onClicked: pictures.rescan()
        }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    Thumbs {
        anchors.fill: parent
        visible: !page.showAlbums
        items: pictures.all
        onPicked: window.view(pictures.all, index)
    }

    GridView {
        id: albums
        anchors { fill: parent; margins: 8 }
        visible: page.showAlbums
        readonly property int columns: Math.max(2, Math.floor(width / 220))
        cellWidth: width / columns
        cellHeight: cellWidth + 56
        model: pictures.albums
        clip: true
        delegate: Item {
            width: albums.cellWidth
            height: albums.cellHeight
            Image {
                id: cover
                x: 8; y: 8
                width: parent.width - 16
                height: width
                source: modelData.cover
                sourceSize.width: width
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                clip: true
                opacity: area.pressed ? 0.6 : 1
            }
            Label {
                anchors { left: cover.left; right: cover.right; top: cover.bottom; topMargin: 4 }
                text: modelData.name + "  (" + modelData.count + ")"
                font.pixelSize: 22
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                color: "white"
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: pageStack.push(Qt.resolvedUrl("AlbumPage.qml"),
                                          { title: modelData.name, folder: modelData.folder })
            }
        }
    }

    Label {
        anchors.centerIn: parent
        visible: pictures.all.length === 0
        text: pictures.scanning ? qsTr("Looking for pictures…") : qsTr("No pictures")
        font.pixelSize: 32
        color: "#8c8c8c"
    }
}
