import QtQuick 2.0
import com.nokia.meego 2.0

// A note written or changed: the whole sheet is its text, the first line
// its title; Save keeps it, Cancel leaves it as it was.
Sheet {
    id: sheet

    property string noteId: ""
    property string original: ""

    acceptButtonText: qsTr("Save")
    rejectButtonText: qsTr("Cancel")
    acceptButtonEnabled: area.text.trim() !== "" && area.text !== original

    function start(id) {
        noteId = id
        original = id !== "" ? notes.text(id) : ""
        area.text = original
        area.cursorPosition = area.text.length
    }

    onStatusChanged: if (status === DialogStatus.Open && noteId === "") area.forceActiveFocus()
    onAccepted: {
        if (notes.save(noteId, area.text) === "") {
            banner.text = qsTr("The note could not be saved")
            banner.show()
        }
        destroy(1000)
    }
    onRejected: destroy(1000)

    content: Item {
        anchors.fill: parent
        Rectangle { anchors.fill: parent; color: "#eeeef0" }
        Flickable {
            id: flick
            anchors.fill: parent
            contentHeight: area.height
            clip: true
            TextArea {
                id: area
                width: flick.width
                height: Math.max(flick.height, implicitHeight)
                placeholderText: qsTr("Write a note")
                textFormat: TextEdit.PlainText
                wrapMode: TextEdit.Wrap
                platformStyle: TextAreaStyle {
                    background: ""
                    backgroundSelected: ""
                    backgroundDisabled: ""
                    textFont.pixelSize: 28
                    paddingLeft: 24
                    paddingRight: 24
                }
            }
        }
        ScrollDecorator { flickableItem: flick }
    }
}
