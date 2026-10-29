import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The call screen, as the N9's: the caller's picture place, the number,
// the call's state or its time; for a call coming in, answer and reject;
// in a call mute, loudspeaker and the keypad for tones, and end.
Page {
    id: page
    objectName: "call"

    readonly property bool landscape: width > height
    readonly property string state: modem.callState
    property bool showPad: false
    property bool ended: false
    property int seconds: 0
    property string lastNumber: modem.callNumber

    // no tool bar: the call's own buttons
    tools: null

    Timer {
        interval: 1000
        repeat: true
        running: page.state === "active"
        onTriggered: page.seconds = Math.floor((new Date() - modem.callActiveSince) / 1000)
    }
    Connections {
        target: modem
        onInCallChanged: {
            if (!modem.inCall) {
                page.ended = true
                closeTimer.start()
            }
        }
        onCallNumberChanged: if (modem.callNumber !== "") page.lastNumber = modem.callNumber
    }
    // "Call ended" for a moment, then back
    Timer {
        id: closeTimer
        interval: 2000
        onTriggered: if (!modem.inCall) pageStack.pop()
    }

    BorderImage {
        anchors.fill: parent
        source: "image://theme/meegotouch-callui-" + (page.landscape ? "landscape" : "portrait") + "-background"
        border { left: 12; right: 12; top: 12; bottom: 12 }
    }
    Rectangle { anchors.fill: parent; color: "#141414"; z: -1 }

    Column {
        id: caller
        anchors { left: parent.left; right: page.landscape ? parent.horizontalCenter : parent.right
                  top: parent.top; topMargin: page.landscape ? 48 : 96 }
        spacing: 12
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            source: "image://theme/icon-l-contact-avatar-placeholder"
            width: 128; height: 128
            smooth: true
        }
        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: page.lastNumber !== "" ? page.lastNumber : qsTr("Unknown number")
            font.pixelSize: 40
            elide: Text.ElideMiddle
            wrapMode: Text.NoWrap
            color: "white"
        }
        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 26
            color: page.state === "active" ? "#5bc23a" : "#c8c8c8"
            text: page.ended ? qsTr("Call ended")
                : page.state === "ringing-in" ? qsTr("Incoming call")
                : page.state === "dialing" || page.state === "ringing-out" ? qsTr("Calling…")
                : page.state === "held" ? qsTr("On hold")
                : page.state === "active" ? Util.duration(page.seconds)
                : ""
        }
    }

    // a call coming in: answer, reject
    Row {
        visible: page.state === "ringing-in" && !page.ended
        anchors { horizontalCenter: page.landscape ? undefined : parent.horizontalCenter
                  right: page.landscape ? parent.right : undefined; rightMargin: 48
                  bottom: parent.bottom; bottomMargin: 48 }
        spacing: 32
        CallButton {
            background: "meegotouch-callui-button-reject-background"
            icon: "icon-m-telephony-call-end"
            text: qsTr("Reject")
            onClicked: modem.hangup()
        }
        CallButton {
            background: "meegotouch-callui-button-answer-background"
            icon: "icon-m-telephony-call"
            text: qsTr("Answer")
            onClicked: modem.accept()
        }
    }

    // in a call: mute, loudspeaker, tones; end
    Column {
        visible: page.state !== "ringing-in" && !page.ended
        anchors { horizontalCenter: page.landscape ? undefined : parent.horizontalCenter
                  right: page.landscape ? parent.right : undefined; rightMargin: 48
                  bottom: parent.bottom; bottomMargin: 32 }
        spacing: 24
        Row {
            spacing: 24
            CallButton {
                background: "meegotouch-callui-button-general-background"
                icon: "icon-m-telephony-mute-button" + (audio.muted ? "-selected" : "")
                text: qsTr("Mute")
                onClicked: audio.muted = !audio.muted
            }
            CallButton {
                background: "meegotouch-callui-button-general-background"
                icon: "icon-m-telephony-handsfree-button" + (audio.speaker ? "-selected" : "")
                        + (audio.canSwitchSpeaker ? "" : "-dimmed")
                text: qsTr("Loudspeaker")
                enabled: audio.canSwitchSpeaker
                onClicked: audio.speaker = !audio.speaker
            }
            CallButton {
                background: "meegotouch-callui-button-general-background"
                icon: "icon-m-telephony-dialer-button" + (page.state === "active" ? "" : "-dimmed")
                text: qsTr("Keypad")
                enabled: page.state === "active"
                onClicked: page.showPad = !page.showPad
            }
        }
        BorderImage {
            width: 3 * 120 + 2 * 24
            height: 80
            source: "image://theme/meegotouch-callui-button-reject-background" + (endArea.pressed ? "-pressed" : "")
            border { left: 24; right: 24; top: 24; bottom: 24 }
            Row {
                anchors.centerIn: parent
                spacing: 12
                Image { source: "image://theme/icon-m-telephony-call-end"; anchors.verticalCenter: parent.verticalCenter }
                Label { text: qsTr("End call"); font.pixelSize: 26; color: "white"; anchors.verticalCenter: parent.verticalCenter }
            }
            MouseArea { id: endArea; anchors.fill: parent; onClicked: modem.hangup() }
        }
    }

    // the tones' keypad, over the caller
    Rectangle {
        visible: page.showPad && page.state === "active" && !page.ended
        anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
        width: page.landscape ? parent.width / 2 : parent.width
        height: page.landscape ? parent.height : parent.height * 0.55
        color: "#e6000000"
        Label {
            id: tones
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: 16 }
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 40
            color: "white"
            text: ""
        }
        Keypad {
            anchors { top: tones.bottom; left: parent.left; right: parent.right; bottom: parent.bottom; margins: 8 }
            onKey: {
                tones.text += k
                modem.sendTone(k)
            }
        }
    }
}
