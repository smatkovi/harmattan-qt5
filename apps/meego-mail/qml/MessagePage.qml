import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// A message: its subject, who wrote it to whom and when, its text, the
// attachments' names; reply, reply to all, forward, delete.
Page {
    id: page

    property var header
    property var message: null

    Component.onCompleted: mail.open(header.uid)
    Connections {
        target: mail
        onOpened: if (uid === page.header.uid) page.message = message
    }

    function reply(all) {
        var m = page.message || page.header
        var to = m.from
        var cc = ""
        if (all) {
            var others = [m.to, m.cc].filter(function (s) { return s && s !== "" }).join(", ")
            cc = others.split(",").filter(function (a) {
                return a.trim() !== "" && a.indexOf(mail.address) < 0
            }).join(",")
        }
        window.compose({ to: to, cc: cc, subject: Util.prefixed("Re:", m.subject || ""),
                         body: "\n\n" + Util.name(m.from) + " " + qsTr("wrote") + ":\n" + Util.quote(m.text || ""),
                         inReplyTo: m.messageId || "" })
    }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon { iconId: "toolbar-reply"; onClicked: page.reply(false) }
        ToolIcon { iconId: "toolbar-reply-all"; onClicked: page.reply(true) }
        ToolIcon {
            iconId: "toolbar-forward"
            onClicked: {
                var m = page.message || page.header
                window.compose({ subject: Util.prefixed("Fwd:", m.subject || ""),
                                 body: "\n\n---- " + qsTr("Forwarded message") + " ----\n"
                                       + qsTr("From") + ": " + m.from + "\n" + qsTr("Subject") + ": " + m.subject
                                       + "\n\n" + (m.text || "") })
            }
        }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: menu.open() }
    }

    Menu {
        id: menu
        MenuLayout {
            MenuItem { text: qsTr("Mark as unread"); onClicked: { mail.markUnread(page.header.uid); pageStack.pop() } }
            MenuItem { text: qsTr("Delete"); onClicked: confirm.open() }
        }
    }
    QueryDialog {
        id: confirm
        titleText: qsTr("Delete message?")
        acceptButtonText: qsTr("Delete")
        rejectButtonText: qsTr("Cancel")
        onAccepted: { mail.remove(page.header.uid); pageStack.pop() }
    }

    Rectangle { anchors.fill: parent; color: "white" }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: column.height + 32
        Column {
            id: column
            width: parent.width
            // the header, as the N9's reader shows it
            Rectangle {
                width: parent.width
                height: head.height + 32
                color: "#eeeeee"
                Column {
                    id: head
                    x: 16; y: 16
                    width: parent.width - 32
                    spacing: 4
                    Label {
                        width: parent.width
                        text: (page.message ? page.message.subject : page.header.subject) || qsTr("(No subject)")
                        font.pixelSize: 30
                        font.bold: true
                        wrapMode: Text.Wrap
                        color: "#282828"
                    }
                    Label {
                        width: parent.width
                        text: qsTr("From") + ": " + page.header.from
                        font.pixelSize: 22
                        wrapMode: Text.Wrap
                        color: "#282828"
                    }
                    Label {
                        width: parent.width
                        visible: (page.message ? page.message.to : page.header.to) !== ""
                        text: qsTr("To") + ": " + (page.message ? page.message.to : page.header.to)
                        font.pixelSize: 20
                        wrapMode: Text.Wrap
                        color: "#6b6b6b"
                    }
                    Label {
                        width: parent.width
                        visible: page.message !== null && page.message.cc !== ""
                        text: qsTr("Cc") + ": " + (page.message ? page.message.cc : "")
                        font.pixelSize: 20
                        wrapMode: Text.Wrap
                        color: "#6b6b6b"
                    }
                    Label {
                        text: Util.full(page.message ? page.message.date : page.header.date)
                        font.pixelSize: 20
                        color: "#8c8c8c"
                    }
                }
            }
            // the attachments
            Repeater {
                model: page.message ? page.message.attachments : []
                Item {
                    width: column.width
                    height: 64
                    BorderImage {
                        anchors.fill: parent
                        source: "image://theme/meegotouch-email-attachment-row"
                        border { left: 10; right: 10; top: 10; bottom: 10 }
                    }
                    Image {
                        id: clip
                        anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
                        source: "image://theme/icon-m-content-attachment"
                    }
                    Label {
                        anchors { left: clip.right; leftMargin: 12; right: parent.right; rightMargin: 16
                                  verticalCenter: parent.verticalCenter }
                        text: modelData
                        font.pixelSize: 22
                        elide: Text.ElideMiddle
                        wrapMode: Text.NoWrap
                        color: "#282828"
                    }
                }
            }
            Item { width: 1; height: 16 }
            Label {
                x: 16
                width: parent.width - 32
                text: page.message ? page.message.text : ""
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                font.pixelSize: 24
                color: "#282828"
            }
        }
    }
    ScrollDecorator { flickableItem: flick }
    BusyIndicator {
        anchors.centerIn: parent
        running: page.message === null
        visible: running
        platformStyle: BusyIndicatorStyle { size: "large" }
    }
}
