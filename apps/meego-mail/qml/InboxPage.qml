import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// A folder's messages, the newest first: the unread ones bold on the N9's
// unread background, the sender, the subject, when. The header names the
// folder and the account; a tap on it changes the folder.
Page {
    id: page

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-new-email"
            enabled: mail.configured
            onClicked: window.compose()
        }
        ToolIcon {
            iconId: "toolbar-refresh"
            enabled: mail.configured && !mail.busy
            opacity: enabled ? 1 : 0.4
            onClicked: mail.refresh()
        }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: menu.open() }
    }

    Menu {
        id: menu
        MenuLayout {
            MenuItem { text: qsTr("Folders"); enabled: mail.configured; onClicked: folderDialog.open() }
            MenuItem { text: qsTr("Account"); onClicked: window.setupAccount() }
        }
    }

    SelectionDialog {
        id: folderDialog
        titleText: qsTr("Folders")
        model: ListModel { id: folderModel }
        onAccepted: mail.openFolder(mail.folders[selectedIndex].name)
        onStatusChanged: {
            if (status === DialogStatus.Opening) {
                folderModel.clear()
                for (var i = 0; i < mail.folders.length; ++i) {
                    folderModel.append({ name: mail.folders[i].title })
                    if (mail.folders[i].name === mail.folder)
                        selectedIndex = i
                }
            }
        }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    // the folder and the account
    Item {
        id: header
        width: parent.width
        height: 80
        BorderImage {
            anchors.fill: parent
            source: "image://theme/meegotouch-email-viewheader-reader"
            border { left: 10; right: 10; top: 10; bottom: 10 }
        }
        Rectangle { anchors.fill: parent; color: "#e2e2e2"; z: -1 }
        Column {
            anchors { left: parent.left; leftMargin: 16; right: busyIndicator.left; rightMargin: 16
                      verticalCenter: parent.verticalCenter }
            Label {
                text: mail.folderTitle + (mail.unread > 0 ? "  (" + mail.unread + ")" : "")
                font.pixelSize: 30
                color: "#282828"
            }
            Label {
                width: parent.width
                text: mail.configured ? mail.address : qsTr("No account")
                font.pixelSize: 20
                color: "#6b6b6b"
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
            }
        }
        BusyIndicator {
            id: busyIndicator
            anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
            running: mail.busy || mail.sending
            visible: running
        }
        MouseArea {
            anchors.fill: parent
            enabled: mail.configured
            onClicked: folderDialog.open()
        }
    }

    ListView {
        id: list
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        clip: true
        model: mail.messages
        delegate: Item {
            width: list.width
            height: 96
            BorderImage {
                anchors.fill: parent
                visible: !modelData.seen
                source: "image://theme/meegotouch-listitem-background-unread" + (area.pressed ? "-pressed" : "")
                border { left: 16; right: 16; top: 16; bottom: 16 }
            }
            Rectangle { anchors.fill: parent; visible: area.pressed && modelData.seen; color: "#d9d9d9" }
            Column {
                anchors { left: parent.left; leftMargin: 16; right: when.left; rightMargin: 16
                          verticalCenter: parent.verticalCenter }
                Label {
                    width: parent.width
                    text: Util.name(modelData.from) || qsTr("(No sender)")
                    font.pixelSize: 26
                    font.bold: !modelData.seen
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "#282828"
                }
                Label {
                    width: parent.width
                    text: modelData.subject || qsTr("(No subject)")
                    font.pixelSize: 22
                    font.bold: !modelData.seen
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: modelData.seen ? "#6b6b6b" : "#282828"
                }
            }
            Label {
                id: when
                anchors { right: parent.right; rightMargin: 16; top: parent.top; topMargin: 18 }
                text: Util.when(modelData.date)
                font.pixelSize: 20
                color: modelData.seen ? "#8c8c8c" : "#3591e7"
            }
            Rectangle {
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
                height: 1
                color: "#d0d0d0"
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: pageStack.push(Qt.resolvedUrl("MessagePage.qml"), { header: modelData })
            }
        }
    }
    ScrollDecorator { flickableItem: list }

    Label {
        anchors.centerIn: list
        visible: list.count === 0
        width: parent.width - 64
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        text: !mail.configured ? qsTr("Tap the menu to set up your mail account")
            : mail.busy ? qsTr("Getting mail…") : qsTr("No messages")
        font.pixelSize: 30
        color: "#b4b4b4"
    }
}
