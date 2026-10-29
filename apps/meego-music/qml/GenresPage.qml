import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

Page {
    id: page

    property var genres: library.genres()

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    ListView {
        id: list
        anchors.fill: parent
        model: page.genres
        header: PageTitle { text: qsTr("Genres") }
        delegate: ListRow {
            icon: "icon-m-content-genre-inverse"
            title: modelData.name
            value: modelData.songs
            drilldown: true
            onClicked: pageStack.push(Qt.resolvedUrl("SongsPage.qml"),
                                      { title: modelData.name, songs: library.genreSongs(modelData.name) })
        }
    }
    ScrollDecorator { flickableItem: list }
}
