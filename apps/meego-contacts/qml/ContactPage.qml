import QtQuick 2.0
import com.nokia.meego 2.0

// A contact's card, as the N9's: the photo and name on the card, then the
// numbers (a tap calls, the message icon writes), the mail addresses, the
// address, the birthday, the note.
Page {
    id: page

    property string uid
    property var c: contacts.contact(uid)
    Connections { target: contacts; onChanged: page.c = contacts.contact(page.uid) }

    function typeName(t) {
        switch (t) {
        case "mobile": return qsTr("Mobile")
        case "work": return qsTr("Work")
        case "home": return qsTr("Home")
        }
        return qsTr("Phone")
    }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon { iconId: "toolbar-edit"; onClicked: window.edit(page.c) }
        ToolIcon { iconId: "toolbar-delete"; onClicked: confirm.open() }
    }

    QueryDialog {
        id: confirm
        titleText: qsTr("Delete contact?")
        message: page.c.name || ""
        acceptButtonText: qsTr("Delete")
        rejectButtonText: qsTr("Cancel")
        onAccepted: { contacts.remove(page.uid); pageStack.pop() }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: column.height + 32
        Column {
            id: column
            width: parent.width
            // the card
            Item {
                width: parent.width
                height: 176
                BorderImage {
                    anchors { fill: parent; margins: 8 }
                    source: "image://theme/meegotouch-contact-card-background"
                    border { left: 16; right: 16; top: 16; bottom: 16 }
                }
                Avatar {
                    id: photo
                    x: 32; y: 24
                    width: 128; height: 128
                    source: page.c.photoUrl || ""
                    placeholder: "icon-m-content-avatar-placeholder-inverse"
                }
                Column {
                    anchors { left: photo.right; leftMargin: 24; right: parent.right; rightMargin: 24
                              verticalCenter: parent.verticalCenter }
                    Label {
                        width: parent.width
                        text: page.c.name || qsTr("(No name)")
                        font.pixelSize: 34
                        wrapMode: Text.Wrap
                        color: "white"
                    }
                    Label {
                        width: parent.width
                        visible: text !== ""
                        text: page.c.org || ""
                        font.pixelSize: 22
                        color: "#c8c8c8"
                    }
                }
            }
            Repeater {
                model: page.c.phones || []
                DetailRow {
                    caption: page.typeName(modelData.type)
                    text: modelData.value
                    icon: "icon-m-telephony-sms"
                    onClicked: contacts.launch("call", modelData.value)
                    onIconClicked: contacts.launch("sms", modelData.value)
                }
            }
            Repeater {
                model: page.c.emails || []
                DetailRow {
                    caption: modelData.type === "work" ? qsTr("Work e-mail") : qsTr("E-mail")
                    text: modelData.value
                    onClicked: contacts.launch("email", modelData.value)
                }
            }
            DetailRow {
                visible: (page.c.address || "") !== ""
                caption: qsTr("Address")
                text: page.c.address || ""
            }
            DetailRow {
                visible: (page.c.birthday || "") !== ""
                caption: qsTr("Birthday")
                text: page.c.birthday ? Qt.formatDate(new Date(page.c.birthday + "T12:00:00"),
                                                      Qt.locale().dateFormat(0)) : ""
            }
            DetailRow {
                visible: (page.c.note || "") !== ""
                caption: qsTr("Note")
                text: page.c.note || ""
            }
        }
    }
    ScrollDecorator { flickableItem: flick }
}
