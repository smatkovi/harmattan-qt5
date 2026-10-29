import QtQuick 2.0
import com.nokia.meego 2.0

// The contacts by name, under their letters, with the N9's letter scroller
// at the side; a search field above (the tool bar's search opens it).
Page {
    id: page

    property string filter: ""
    property var list: contacts.contacts(filter)
    Connections { target: contacts; onChanged: page.list = contacts.contacts(page.filter) }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-add"; onClicked: window.edit(null) }
        ToolIcon {
            iconId: "toolbar-search"
            onClicked: {
                search.visible = !search.visible
                if (search.visible) search.forceActiveFocus(); else search.text = ""
            }
        }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: menu.open() }
    }

    Menu {
        id: menu
        MenuLayout {
            MenuItem {
                text: qsTr("Import contacts")
                onClicked: {
                    var n = contacts.importFiles()
                    banner.text = n > 0 ? qsTr("%1 contacts imported").arg(n)
                                        : qsTr("No new contacts in the .vcf files of Documents or Downloads")
                    banner.show()
                }
            }
        }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    TextField {
        id: search
        visible: false
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: 8 }
        height: visible ? implicitHeight : 0
        placeholderText: qsTr("Search")
        inputMethodHints: Qt.ImhNoPredictiveText
        onTextChanged: page.filter = text
    }

    ListView {
        id: view
        anchors { top: search.visible ? search.bottom : parent.top; topMargin: search.visible ? 8 : 0
                  left: parent.left; right: parent.right; bottom: parent.bottom }
        clip: true
        model: ListModel { id: model }
        section.property: "section"
        section.criteria: ViewSection.FullString
        section.delegate: Item {
            width: view.width
            height: 40
            Label {
                anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
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
            height: 88
            Rectangle { anchors.fill: parent; color: area.pressed ? "#d9d9d9" : "transparent" }
            Avatar {
                id: avatar
                anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
                width: 64; height: 64
                source: model.photoUrl
            }
            Column {
                anchors { left: avatar.right; leftMargin: 16; right: parent.right; rightMargin: 48
                          verticalCenter: parent.verticalCenter }
                Label {
                    width: parent.width
                    text: model.name || qsTr("(No name)")
                    font.pixelSize: 26
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#282828"
                }
                Label {
                    width: parent.width
                    visible: text !== ""
                    text: model.org
                    font.pixelSize: 20
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#8c8c8c"
                }
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: pageStack.push(Qt.resolvedUrl("ContactPage.qml"), { uid: model.uid })
            }
        }
    }
    SectionScroller { listView: view }
    ScrollDecorator { flickableItem: view }

    // the list as a ListModel, for the sections and the scroller
    onListChanged: {
        model.clear()
        for (var i = 0; i < list.length; ++i)
            model.append({ uid: list[i].uid, name: list[i].name, org: list[i].org || "",
                           section: list[i].section, photoUrl: list[i].photoUrl })
    }
    Component.onCompleted: listChanged()

    Label {
        anchors.centerIn: parent
        visible: page.list.length === 0
        width: parent.width - 64
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        text: page.filter !== "" ? qsTr("No matches") : qsTr("No contacts")
        font.pixelSize: 32
        color: "#b4b4b4"
    }
}
