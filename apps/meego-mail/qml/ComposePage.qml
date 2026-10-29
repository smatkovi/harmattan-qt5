import QtQuick 2.0
import com.nokia.meego 2.0

// A new message, a reply or a forward: to, cc, subject, the text; Send.
Page {
    id: page

    property string to
    property string cc
    property string subject
    property string body
    property string inReplyTo

    function send() {
        mail.send(toField.text, ccField.text, subjectField.text, bodyArea.text, page.inReplyTo)
        pageStack.pop()
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: bodyArea.text !== page.body || toField.text !== page.to ? discard.open() : pageStack.pop()
        }
        ToolIcon {
            iconId: "toolbar-send-email"
            enabled: toField.text.indexOf("@") > 0
            opacity: enabled ? 1 : 0.4
            onClicked: page.send()
        }
    }

    QueryDialog {
        id: discard
        titleText: qsTr("Discard the message?")
        acceptButtonText: qsTr("Discard")
        rejectButtonText: qsTr("Cancel")
        onAccepted: pageStack.pop()
    }

    Rectangle { anchors.fill: parent; color: "white" }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: column.height + 32
        Column {
            id: column
            x: 16; y: 16
            width: parent.width - 32
            spacing: 12
            Row {
                width: parent.width
                spacing: 12
                Label { id: toLabel; width: 64; text: qsTr("To"); font.pixelSize: 24; anchors.verticalCenter: parent.verticalCenter; color: "#6b6b6b" }
                TextField {
                    id: toField
                    width: parent.width - toLabel.width - 12
                    text: page.to
                    inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase
                }
            }
            Row {
                width: parent.width
                spacing: 12
                Label { width: 64; text: qsTr("Cc"); font.pixelSize: 24; anchors.verticalCenter: parent.verticalCenter; color: "#6b6b6b" }
                TextField {
                    id: ccField
                    width: parent.width - 64 - 12
                    text: page.cc
                    inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase
                }
            }
            TextField {
                id: subjectField
                width: parent.width
                placeholderText: qsTr("Subject")
                text: page.subject
            }
            TextArea {
                id: bodyArea
                width: parent.width
                height: Math.max(240, implicitHeight)
                placeholderText: qsTr("Message")
                text: page.body
            }
        }
    }
    ScrollDecorator { flickableItem: flick }
}
