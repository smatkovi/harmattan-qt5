import QtQuick 2.0
import com.nokia.meego 2.0

// A new message: to whom (a number), the text, Send; then its conversation.
Page {
    id: page

    property string text: ""

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon {
            iconId: "toolbar-send-sms"
            enabled: to.text !== "" && body.text !== ""
            opacity: enabled ? 1 : 0.4
            onClicked: {
                if (!sms.send(to.text, body.text)) {
                    banner.text = sms.available ? qsTr("Not sent: %1").arg(sms.error())
                                                : qsTr("No modem: messages cannot be sent")
                    banner.show()
                    return
                }
                var number = to.text
                pageStack.pop()
                window.openConversation(number)
            }
        }
    }

    InfoBanner { id: banner; z: 10 }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    Column {
        anchors { fill: parent; margins: 16 }
        spacing: 16
        Label {
            text: qsTr("New message")
            font.pixelSize: 32
            color: "#282828"
        }
        TextField {
            id: to
            width: parent.width
            placeholderText: qsTr("To")
            inputMethodHints: Qt.ImhDialableCharactersOnly
        }
        TextArea {
            id: body
            width: parent.width
            height: Math.max(160, implicitHeight)
            placeholderText: qsTr("Message")
            text: page.text
        }
    }
}
