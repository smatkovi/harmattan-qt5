import QtQuick 2.0
import com.nokia.meego 2.0

// The start: the bookmarks, then the pages seen lately; a tap opens one,
// a long press removes a bookmark.
Item {
    id: start

    signal open(string url)

    ListModel { id: model }
    function fill() {
        model.clear()
        var b = store.bookmarks, h = store.history
        for (var i = 0; i < b.length; ++i)
            model.append({ section: qsTr("Bookmarks"), url: b[i].url, title: b[i].title, bookmark: true })
        for (var j = 0; j < Math.min(h.length, 30); ++j)
            model.append({ section: qsTr("Recent"), url: h[j].url, title: h[j].title, bookmark: false })
    }
    Connections { target: store; onChanged: start.fill() }
    Component.onCompleted: fill()

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    ListView {
        id: view
        anchors.fill: parent
        clip: true
        model: model
        section.property: "section"
        section.delegate: Item {
            width: view.width
            height: 48
            Label {
                anchors { left: parent.left; leftMargin: 16; bottom: parent.bottom; bottomMargin: 8 }
                text: section
                font.pixelSize: 22
                font.bold: true
                color: "#3591e7"
            }
            Rectangle {
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
                height: 1
                color: "#3591e7"
            }
        }
        delegate: Item {
            width: view.width
            height: 80
            Rectangle { anchors.fill: parent; color: area.pressed ? "#d9d9d9" : "transparent" }
            Image {
                id: icon
                anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
                source: "image://theme/" + (model.bookmark ? "icon-m-toolbar-favorite-mark" : "icon-m-toolbar-history")
            }
            Column {
                anchors { left: icon.right; leftMargin: 16; right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
                Label {
                    width: parent.width
                    text: model.title
                    font.pixelSize: 24
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#282828"
                }
                Label {
                    width: parent.width
                    text: model.url
                    font.pixelSize: 18
                    elide: Text.ElideMiddle
                    wrapMode: Text.NoWrap
                    color: "#8c8c8c"
                }
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: start.open(model.url)
                onPressAndHold: if (model.bookmark) { menu.url = model.url; menu.open() }
            }
        }
    }
    ScrollDecorator { flickableItem: view }

    Label {
        anchors.centerIn: parent
        visible: model.count === 0
        width: parent.width - 64
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        text: qsTr("Write an address or words to search for above")
        font.pixelSize: 32
        color: "#b4b4b4"
    }

    ContextMenu {
        id: menu
        property string url
        MenuLayout {
            MenuItem { text: qsTr("Remove bookmark"); onClicked: store.removeBookmark(menu.url) }
        }
    }
}
