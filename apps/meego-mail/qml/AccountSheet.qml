import QtQuick 2.0
import com.nokia.meego 2.0

// The mail account: name, address, password; the servers filled in from
// the address (known providers, else imap./smtp. of its domain), to change.
Sheet {
    id: sheet

    property var a: mail.account()
    property var securities: ["ssl", "starttls", "none"]

    acceptButtonText: qsTr("Save")
    rejectButtonText: qsTr("Cancel")
    acceptButtonEnabled: address.text.indexOf("@") > 0 && imapHost.text !== "" && smtpHost.text !== ""

    function fillServers() {
        var g = mail.guess(address.text)
        if (!g.imapHost)
            return
        imapHost.text = g.imapHost
        imapPort.text = g.imapPort
        imapSec.checkedButton = imapSec.children[securities.indexOf(g.imapSecurity)]
        smtpHost.text = g.smtpHost
        smtpPort.text = g.smtpPort
        smtpSec.checkedButton = smtpSec.children[securities.indexOf(g.smtpSecurity)]
    }

    onAccepted: {
        mail.setup({ name: name.text, address: address.text, user: user.text, password: password.text,
                     imapHost: imapHost.text, imapPort: parseInt(imapPort.text),
                     imapSecurity: imapSec.checkedButton ? imapSec.checkedButton.key : "ssl",
                     smtpHost: smtpHost.text, smtpPort: parseInt(smtpPort.text),
                     smtpSecurity: smtpSec.checkedButton ? smtpSec.checkedButton.key : "ssl" })
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

                Label { text: qsTr("Mail account"); font.pixelSize: 32; color: "#282828" }
                TextField { id: name; width: parent.width; placeholderText: qsTr("Your name"); text: sheet.a.name }
                TextField {
                    id: address
                    width: parent.width
                    placeholderText: qsTr("E-mail address")
                    text: sheet.a.address
                    inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase
                    onActiveFocusChanged: if (!activeFocus && imapHost.text === "") sheet.fillServers()
                }
                TextField {
                    id: password
                    width: parent.width
                    placeholderText: qsTr("Password")
                    echoMode: TextInput.Password
                    text: sheet.a.password
                }
                Button {
                    text: qsTr("Fill in the servers")
                    enabled: address.text.indexOf("@") > 0
                    onClicked: sheet.fillServers()
                }

                Label { text: qsTr("Incoming (IMAP)"); font.pixelSize: 24; color: "#3591e7" }
                TextField { id: imapHost; width: parent.width; placeholderText: qsTr("Server"); text: sheet.a.imapHost
                            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase }
                Row {
                    width: parent.width
                    spacing: 12
                    TextField { id: imapPort; width: 120; text: sheet.a.imapPort; inputMethodHints: Qt.ImhDigitsOnly }
                    ButtonRow {
                        id: imapSec
                        width: parent.width - 132
                        Button { property string key: "ssl"; text: "SSL"; checked: sheet.a.imapSecurity === key }
                        Button { property string key: "starttls"; text: "STARTTLS"; checked: sheet.a.imapSecurity === key }
                        Button { property string key: "none"; text: qsTr("None"); checked: sheet.a.imapSecurity === key }
                    }
                }
                TextField { id: user; width: parent.width; placeholderText: qsTr("User name (the address if empty)")
                            text: sheet.a.user !== sheet.a.address ? sheet.a.user : ""
                            inputMethodHints: Qt.ImhNoAutoUppercase }

                Label { text: qsTr("Outgoing (SMTP)"); font.pixelSize: 24; color: "#3591e7" }
                TextField { id: smtpHost; width: parent.width; placeholderText: qsTr("Server"); text: sheet.a.smtpHost
                            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase }
                Row {
                    width: parent.width
                    spacing: 12
                    TextField { id: smtpPort; width: 120; text: sheet.a.smtpPort; inputMethodHints: Qt.ImhDigitsOnly }
                    ButtonRow {
                        id: smtpSec
                        width: parent.width - 132
                        Button { property string key: "ssl"; text: "SSL"; checked: sheet.a.smtpSecurity === key }
                        Button { property string key: "starttls"; text: "STARTTLS"; checked: sheet.a.smtpSecurity === key }
                        Button { property string key: "none"; text: qsTr("None"); checked: sheet.a.smtpSecurity === key }
                    }
                }
                Label {
                    width: parent.width
                    wrapMode: Text.Wrap
                    text: qsTr("The password is kept in ~/.config/meego/meego-mail.conf, readable by you alone.")
                    font.pixelSize: 18
                    color: "#8c8c8c"
                }
            }
        }
        ScrollDecorator { flickableItem: flick }
    }
}
