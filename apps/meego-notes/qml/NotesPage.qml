import QtQuick 2.0
import com.nokia.meego 2.0

// The notes, the newest first: each its first line and below it a little
// of the rest, its time at the side; a tap writes in it, a long press
// deletes it. The tool bar's search shows only the notes with its words.
Page {
    id: page

    property string filter: ""
    property var list: notes.find(filter)
    // assigned, not bound: the filter's changes come through its handler
    onFilterChanged: list = notes.find(filter)
    Connections { target: notes; onChanged: page.list = notes.find(page.filter) }

    function when(d) {
        var now = new Date()
        if (d.toDateString() === now.toDateString())
            return Qt.formatTime(d, "hh:mm")
        return d.getFullYear() === now.getFullYear() ? Qt.formatDate(d, "d MMM") : Qt.formatDate(d, "d MMM yyyy")
    }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-add"; onClicked: window.edit("") }
        ToolIcon {
            iconId: "toolbar-search"
            onClicked: {
                search.visible = !search.visible
                if (search.visible) search.forceActiveFocus(); else search.text = ""
            }
        }
    }

    Rectangle { anchors.fill: parent; color: "#e0e1e3" }

    // the N9's notes header is yellow
    PageHeader {
        id: header
        color: "#f2b011"
        text: qsTr("Notes")
    }

    TextField {
        id: search
        visible: false
        anchors { top: header.bottom; left: parent.left; right: parent.right; margins: 8 }
        height: visible ? implicitHeight : 0
        placeholderText: qsTr("Search")
        inputMethodHints: Qt.ImhNoPredictiveText
        onTextChanged: page.filter = text
    }

    ListView {
        id: view
        anchors { top: search.visible ? search.bottom : header.bottom; topMargin: search.visible ? 8 : 0
                  left: parent.left; right: parent.right; bottom: parent.bottom }
        clip: true
        model: page.list
        delegate: Item {
            width: view.width
            height: column.height + 32
            Rectangle { anchors.fill: parent; color: area.pressed ? "#cfd0d2" : "transparent" }
            Column {
                id: column
                anchors { left: parent.left; leftMargin: 16; right: time.left; rightMargin: 16
                          verticalCenter: parent.verticalCenter }
                spacing: 2
                Label {
                    width: parent.width
                    text: modelData.title
                    font.pixelSize: 26
                    font.bold: true
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#282828"
                }
                Label {
                    width: parent.width
                    visible: text !== ""
                    text: modelData.rest
                    font.pixelSize: 22
                    maximumLineCount: 2
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    textFormat: Text.PlainText
                    color: "#6e6e6e"
                }
            }
            Label {
                id: time
                anchors { right: parent.right; rightMargin: 16; top: column.top; topMargin: 4 }
                text: page.when(modelData.modified)
                font.pixelSize: 20
                color: "#8c8c8c"
            }
            Rectangle {
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
                height: 1
                color: "#c8c8ca"
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: window.edit(modelData.id)
                onPressAndHold: {
                    menu.noteId = modelData.id
                    menu.noteTitle = modelData.title
                    menu.open()
                }
            }
        }
    }
    ScrollDecorator { flickableItem: view }

    Column {
        anchors.centerIn: view
        visible: page.list.length === 0
        spacing: 32
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: page.filter !== "" ? qsTr("No matches") : qsTr("No notes")
            font.pixelSize: 32
            color: "#b4b4b4"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: page.filter === ""
            text: qsTr("Add note")
            onClicked: window.edit("")
        }
    }

    ContextMenu {
        id: menu
        property string noteId
        property string noteTitle
        MenuLayout {
            MenuItem { text: qsTr("Delete"); onClicked: confirm.open() }
        }
    }
    QueryDialog {
        id: confirm
        titleText: qsTr("Delete note?")
        message: menu.noteTitle
        acceptButtonText: qsTr("Delete")
        rejectButtonText: qsTr("Cancel")
        onAccepted: notes.remove(menu.noteId)
    }
}
