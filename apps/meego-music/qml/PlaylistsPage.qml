import QtQuick 2.0
import com.nokia.meego 2.0

// The smart playlists, as the N9 had them.
Page {
    id: page

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    ListView {
        id: list
        anchors.fill: parent
        header: PageTitle { text: qsTr("Playlists") }
        model: ListModel {
            ListElement { key: "recent"; name: "Recently added" }
            ListElement { key: "played"; name: "Recently played" }
            ListElement { key: "most"; name: "Most played" }
        }
        delegate: ListRow {
            icon: "icon-m-content-playlist-inverse"
            title: qsTr(model.name)
            value: { library.count; return library.playlist(model.key).length }
            drilldown: true
            onClicked: pageStack.push(Qt.resolvedUrl("SongsPage.qml"),
                                      { title: qsTr(model.name), songs: library.playlist(model.key) })
        }
    }
}
