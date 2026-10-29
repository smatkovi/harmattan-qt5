import QtQuick 2.0
import com.nokia.meego 2.0

// A new or changed contact: names, organisation, up to three numbers with
// their kind, two mail addresses, the address, the birthday, a note.
Sheet {
    id: sheet

    property string uid: ""
    property var kinds: ["mobile", "home", "work", "phone"]
    property var birthday: undefined

    acceptButtonText: qsTr("Save")
    rejectButtonText: qsTr("Cancel")
    acceptButtonEnabled: first.text !== "" || last.text !== "" || org.text !== ""

    function start(c) {
        if (!c)
            return
        uid = c.uid
        first.text = c.first || ""
        last.text = c.last || ""
        org.text = c.org || ""
        var p = c.phones || []
        for (var i = 0; i < 3; ++i) {
            phones.itemAt(i).value = i < p.length ? p[i].value : ""
            phones.itemAt(i).kind = i < p.length ? p[i].type : "mobile"
        }
        var e = c.emails || []
        mail1.text = e.length > 0 ? e[0].value : ""
        mail2.text = e.length > 1 ? e[1].value : ""
        address.text = c.address || ""
        birthday = c.birthday ? new Date(c.birthday + "T12:00:00") : undefined
        note.text = c.note || ""
    }

    onAccepted: {
        var p = []
        for (var i = 0; i < 3; ++i)
            if (phones.itemAt(i).value !== "")
                p.push({ type: phones.itemAt(i).kind, value: phones.itemAt(i).value })
        var e = []
        if (mail1.text !== "") e.push({ type: "home", value: mail1.text })
        if (mail2.text !== "") e.push({ type: "work", value: mail2.text })
        contacts.save({ uid: uid, first: first.text, last: last.text, org: org.text, phones: p, emails: e,
                        address: address.text,
                        birthday: birthday ? Qt.formatDate(birthday, "yyyy-MM-dd") : "",
                        note: note.text })
        destroy(1000)
    }
    onRejected: destroy(1000)

    content: Item {
        anchors.fill: parent
        Rectangle { anchors.fill: parent; color: "#f2f2f2" }
        Flickable {
            id: flick
            anchors.fill: parent
            contentHeight: column.height + 32
            clip: true
            Column {
                id: column
                x: 16; y: 16
                width: parent.width - 32
                spacing: 12
                TextField { id: first; width: parent.width; placeholderText: qsTr("First name") }
                TextField { id: last; width: parent.width; placeholderText: qsTr("Last name") }
                TextField { id: org; width: parent.width; placeholderText: qsTr("Company") }
                Repeater {
                    id: phones
                    model: 3
                    Row {
                        property alias value: number.text
                        property string kind: "mobile"
                        width: column.width
                        spacing: 12
                        Button {
                            width: 180
                            text: kind === "mobile" ? qsTr("Mobile") : kind === "home" ? qsTr("Home")
                                : kind === "work" ? qsTr("Work") : qsTr("Phone")
                            onClicked: kind = sheet.kinds[(sheet.kinds.indexOf(kind) + 1) % sheet.kinds.length]
                        }
                        TextField {
                            id: number
                            width: parent.width - 192
                            placeholderText: qsTr("Phone number")
                            inputMethodHints: Qt.ImhDialableCharactersOnly
                        }
                    }
                }
                TextField { id: mail1; width: parent.width; placeholderText: qsTr("E-mail")
                            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase }
                TextField { id: mail2; width: parent.width; placeholderText: qsTr("Work e-mail")
                            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase }
                TextField { id: address; width: parent.width; placeholderText: qsTr("Address") }
                Row {
                    width: parent.width
                    spacing: 12
                    Button {
                        width: parent.width - clearBirthday.width - 12
                        text: sheet.birthday ? qsTr("Birthday: %1").arg(Qt.formatDate(sheet.birthday, Qt.locale().dateFormat(1)))
                                             : qsTr("Birthday")
                        onClicked: {
                            var d = sheet.birthday || new Date(1990, 0, 1)
                            picker.year = d.getFullYear()
                            picker.month = d.getMonth() + 1
                            picker.day = d.getDate()
                            picker.open()
                        }
                    }
                    Button {
                        id: clearBirthday
                        width: 80
                        text: "×"
                        enabled: sheet.birthday !== undefined
                        onClicked: sheet.birthday = undefined
                    }
                }
                TextArea { id: note; width: parent.width; height: Math.max(120, implicitHeight); placeholderText: qsTr("Note") }
            }
        }
        ScrollDecorator { flickableItem: flick }
    }

    DatePickerDialog {
        id: picker
        titleText: qsTr("Birthday")
        acceptButtonText: qsTr("Done")
        rejectButtonText: qsTr("Cancel")
        minimumYear: 1900
        maximumYear: new Date().getFullYear()
        onAccepted: sheet.birthday = new Date(year, month - 1, day, 12)
    }
}
