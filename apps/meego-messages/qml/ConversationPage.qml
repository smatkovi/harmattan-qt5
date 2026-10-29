import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// A conversation: the messages in the N9's bubbles (theirs blue on the
// right, one's own white on the left, as the N9's pictures point), the
// time under each, the writing field and Send at the bottom.
Page {
    id: page

    property string number
    property var list: sms.messages(number)

    function reload() {
        list = sms.messages(number)
        sms.markRead(number)
        view.positionViewAtEnd()
    }
    function sendText() {
        var t = field.text
        if (t === "")
            return
        if (!sms.send(page.number, t)) {
            banner.text = sms.available ? qsTr("Not sent: %1").arg(sms.error())
                                        : qsTr("No modem: messages cannot be sent")
            banner.show()
            return
        }
        field.text = ""
    }

    Component.onCompleted: { sms.markRead(number); view.positionViewAtEnd() }
    Connections { target: sms; onChanged: page.reload() }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon {
            iconId: "toolbar-send-sms"
            enabled: field.text !== ""
            opacity: enabled ? 1 : 0.4
            onClicked: page.sendText()
        }
    }

    InfoBanner { id: banner; z: 10 }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    // who
    Item {
        id: header
        width: parent.width
        height: 72
        Rectangle { anchors.fill: parent; color: "#e2e2e2" }
        Label {
            anchors { left: parent.left; leftMargin: 16; right: parent.right; rightMargin: 16
                      verticalCenter: parent.verticalCenter }
            text: page.number
            font.pixelSize: 28
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            color: "#282828"
        }
    }

    ListView {
        id: view
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: composer.top }
        clip: true
        spacing: 8
        model: page.list
        header: Item { height: 8; width: 1 }
        footer: Item { height: 8; width: 1 }
        delegate: Item {
            id: msg
            width: view.width
            height: bubble.height + time.height + 4
            readonly property bool theirs: modelData.incoming
            BorderImage {
                id: bubble
                x: msg.theirs ? view.width - width - 16 : 16
                width: Math.min(view.width * 0.75, Math.max(text.implicitWidth, 80) + 48)
                height: text.paintedHeight + 32
                source: "image://theme/meegotouch-messaging-conversation-bubble-"
                        + (msg.theirs ? "incoming1" : "outgoing1") + (area.pressed ? "-pressed" : "-normal")
                border { left: msg.theirs ? 16 : 28; right: msg.theirs ? 28 : 16; top: 24; bottom: 24 }
                Text {
                    id: text
                    anchors { left: parent.left; right: parent.right; top: parent.top
                              leftMargin: msg.theirs ? 20 : 28; rightMargin: msg.theirs ? 28 : 20; topMargin: 14 }
                    text: modelData.text
                    wrapMode: Text.Wrap
                    font.pixelSize: 24
                    font.family: "Nokia Pure Text"
                    color: msg.theirs ? "white" : "#282828"
                }
                MouseArea {
                    id: area
                    anchors.fill: parent
                    onPressAndHold: { menu.id = modelData.id; menu.text = modelData.text; menu.open() }
                }
            }
            Label {
                id: time
                anchors { top: bubble.bottom; topMargin: 2
                          right: msg.theirs ? bubble.right : undefined; left: msg.theirs ? undefined : bubble.left
                          rightMargin: 8; leftMargin: 8 }
                text: Util.stamp(modelData.time)
                      + (modelData.status === "sending" ? "  ·  " + qsTr("Sending…")
                         : modelData.status === "failed" ? "  ·  " + qsTr("Not sent") : "")
                font.pixelSize: 18
                color: modelData.status === "failed" ? "#e4413a" : "#8c8c8c"
            }
        }
    }
    ScrollDecorator { flickableItem: view }

    // writing
    Item {
        id: composer
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: Math.min(field.implicitHeight, 200) + 16
        Rectangle { anchors.fill: parent; color: "#e2e2e2" }
        TextArea {
            id: field
            anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; margins: 8 }
            placeholderText: sms.available ? qsTr("Write a message") : qsTr("No modem")
            height: Math.min(implicitHeight, 200)
        }
    }

    ContextMenu {
        id: menu
        property int id
        property string text
        MenuLayout {
            MenuItem { text: qsTr("Forward"); onClicked: pageStack.push(Qt.resolvedUrl("NewMessagePage.qml"), { text: menu.text }) }
            MenuItem { text: qsTr("Delete"); onClicked: sms.removeMessage(menu.id) }
        }
    }
}
