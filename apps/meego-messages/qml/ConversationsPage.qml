import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The conversations, the latest first: who, the last message, when, the
// unread count. A long press deletes one.
Page {
    id: page

    property string number: ""   // not a conversation (for the banner)
    property var list: sms.conversations()
    Connections { target: sms; onChanged: page.list = sms.conversations() }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-new-message"
            onClicked: pageStack.push(Qt.resolvedUrl("NewMessagePage.qml"))
        }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    ListView {
        id: view
        anchors.fill: parent
        model: page.list
        header: Item {
            width: view.width
            height: 80
            Label {
                anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
                text: qsTr("Conversations")
                font.pixelSize: 32
                color: "#282828"
            }
            Label {
                anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
                visible: !sms.available
                text: qsTr("No modem")
                font.pixelSize: 22
                color: "#e4413a"
            }
        }
        delegate: Item {
            width: view.width
            height: 104
            Rectangle { anchors.fill: parent; color: area.pressed ? "#d9d9d9" : "transparent" }
            Image {
                id: avatar
                anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
                source: "image://theme/icon-m-content-avatar-placeholder"
                width: 64; height: 64
                smooth: true
            }
            Column {
                anchors { left: avatar.right; leftMargin: 16; right: right.left; rightMargin: 16
                          verticalCenter: parent.verticalCenter }
                Label {
                    width: parent.width
                    text: modelData.number
                    font.pixelSize: 26
                    font.bold: true
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#282828"
                }
                Label {
                    width: parent.width
                    text: (modelData.incoming ? "" : qsTr("You: ")) + modelData.text.replace(/\n/g, " ")
                    font.pixelSize: 22
                    font.bold: modelData.unread > 0
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: modelData.unread > 0 ? "#282828" : "#6b6b6b"
                }
            }
            Column {
                id: right
                anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
                spacing: 6
                Label {
                    anchors.right: parent.right
                    text: Util.when(modelData.time)
                    font.pixelSize: 20
                    color: modelData.unread > 0 ? "#3591e7" : "#8c8c8c"
                }
                CountBubble {
                    anchors.right: parent.right
                    visible: modelData.unread > 0
                    value: modelData.unread
                    largeSized: true
                }
            }
            Rectangle {
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
                height: 1
                color: "#d0d0d0"
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: window.openConversation(modelData.number)
                onPressAndHold: { menu.number = modelData.number; menu.open() }
            }
        }
    }
    ScrollDecorator { flickableItem: view }

    Label {
        anchors.centerIn: parent
        visible: page.list.length === 0
        text: qsTr("No conversations")
        font.pixelSize: 32
        color: "#b4b4b4"
    }

    ContextMenu {
        id: menu
        property string number
        MenuLayout {
            MenuItem { text: qsTr("Delete conversation"); onClicked: confirm.open() }
        }
    }
    QueryDialog {
        id: confirm
        titleText: qsTr("Delete conversation?")
        message: menu.number
        acceptButtonText: qsTr("Delete")
        rejectButtonText: qsTr("Cancel")
        onAccepted: sms.removeConversation(menu.number)
    }
}
