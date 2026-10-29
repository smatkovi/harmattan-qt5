import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The dialler, as on the N9: the number in its field with the delete key,
// the keypad and the green call key; the latest calls beside the keypad
// (landscape). Without a modem a banner says so instead of calling.
Page {
    id: page

    property string number: startNumber
    readonly property bool landscape: width > height

    function call() {
        if (number === "") {
            // the N9 puts the last number back on an empty call key
            if (history.calls.length > 0)
                number = history.calls[0].number
            return
        }
        if (!modem.dial(number)) {
            banner.text = modem.available ? qsTr("The call did not start: %1").arg(modem.error())
                                          : qsTr("No modem: calls are not possible")
            banner.show()
            return
        }
        number = ""
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-callhistory"
            onClicked: pageStack.push(Qt.resolvedUrl("HistoryPage.qml"), { dialer: page })
            CountBubble {
                anchors { right: parent.right; top: parent.top; margins: 4 }
                value: history.missed
                visible: value > 0
                largeSized: false
            }
        }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: menu.open() }
    }

    Menu {
        id: menu
        MenuLayout {
            MenuItem {
                text: modem.available
                      ? (modem.operatorName !== "" ? modem.operatorName : qsTr("Modem")) + "  ·  " + modem.name
                      : qsTr("No modem")
                enabled: false
            }
            MenuItem { text: qsTr("Clear call history"); onClicked: history.clear() }
        }
    }

    InfoBanner { id: banner; z: 10 }

    Rectangle { anchors.fill: parent; color: "black" }

    // the number and its delete key
    Item {
        id: field
        x: 0
        y: 0
        width: page.landscape ? page.width * 0.45 : page.width
        height: page.landscape ? 120 : 150
        BorderImage {
            anchors.fill: parent
            source: "image://theme/meegotouch-callui-inputfield-general-background"
            border { left: 8; right: 8; top: 8; bottom: 8 }
        }
        Label {
            id: numberLabel
            anchors { left: parent.left; right: erase.left; leftMargin: 24; rightMargin: 8; verticalCenter: parent.verticalCenter }
            text: page.number !== "" ? Util.display(page.number) : (modem.available ? "" : qsTr("No modem"))
            font.pixelSize: page.number.length > 12 ? 40 : 56
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideLeft
            wrapMode: Text.NoWrap
            color: page.number !== "" ? "white" : "#6b6b6b"
        }
        Image {
            id: erase
            visible: page.number !== ""
            anchors { right: parent.right; rightMargin: 24; verticalCenter: parent.verticalCenter }
            source: "image://theme/icon-m-telephony-backspace" + (eraseArea.pressed ? "-selected" : "")
            MouseArea {
                id: eraseArea
                anchors { fill: parent; margins: -24 }
                onClicked: page.number = page.number.slice(0, -1)
                onPressAndHold: page.number = ""
            }
        }
    }

    // the latest calls (landscape)
    ListView {
        id: recent
        visible: page.landscape
        anchors { left: parent.left; top: field.bottom; bottom: parent.bottom }
        width: field.width
        clip: true
        model: history.calls
        delegate: CallRow {
            call: modelData
            onClicked: page.number = modelData.number
        }
        Label {
            anchors.centerIn: parent
            visible: history.calls.length === 0
            text: qsTr("No calls")
            font.pixelSize: 28
            color: "#6b6b6b"
        }
    }

    Keypad {
        id: pad
        anchors { right: parent.right; top: page.landscape ? parent.top : field.bottom
                  bottom: callKey.top; margins: 8 }
        width: page.landscape ? page.width * 0.55 - 16 : page.width - 16
        onKey: page.number += k
    }

    // the green call key
    BorderImage {
        id: callKey
        anchors { right: parent.right; bottom: parent.bottom; margins: 8 }
        width: pad.width
        height: page.landscape ? 72 : 96
        source: "image://theme/meegotouch-callui-dialler-ok-button-background" + (callArea.pressed ? "-pressed" : "")
        border { left: 20; right: 20; top: 20; bottom: 20 }
        Image {
            anchors.centerIn: parent
            source: "image://theme/icon-m-telephony-call"
        }
        MouseArea {
            id: callArea
            anchors.fill: parent
            onClicked: page.call()
        }
    }
}
