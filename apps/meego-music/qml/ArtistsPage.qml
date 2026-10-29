import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

Page {
    id: page

    property var artists: library.artists()

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    ListView {
        id: list
        anchors.fill: parent
        model: page.artists
        header: PageTitle { text: qsTr("Artists") }
        delegate: ListRow {
            showCover: true
            cover: modelData.cover
            title: modelData.name
            subtitle: Util.count(modelData.albums, qsTr("1 album"), qsTr("%1 albums")) + ", "
                      + Util.count(modelData.songs, qsTr("1 song"), qsTr("%1 songs"))
            drilldown: true
            onClicked: pageStack.push(Qt.resolvedUrl("AlbumsPage.qml"), { artist: modelData.name })
        }
    }
    ScrollDecorator { flickableItem: list }
}
