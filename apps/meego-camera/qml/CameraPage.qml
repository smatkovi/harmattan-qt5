import QtQuick 2.0
import QtQuick.Window 2.2 as QtWindow
import com.nokia.meego 2.0
import MeeGo.Camera 1.0

// The viewfinder full screen, as the N9's: at the side the switch between
// photo and video above, the shutter key in the middle, the last picture
// below (a tap shows it in the gallery or the videos); the other camera at
// the top left where there are two; a video's time at the top. The camera
// is off while the application is in the background. Space, Enter or a
// camera key takes the picture too.
Page {
    id: page

    property bool video: false

    orientationLock: PageOrientation.LockLandscape

    function shutter() {
        if (!vf.running || vf.busy)
            return
        if (!page.video)
            vf.capture()
        else if (vf.recording)
            vf.stopRecording()
        else
            vf.startRecording()
    }
    function time(s) {
        var m = Math.floor(s / 60), h = Math.floor(m / 60)
        var ss = (s % 60 < 10 ? "0" : "") + (s % 60)
        return h > 0 ? h + ":" + (m % 60 < 10 ? "0" : "") + (m % 60) + ":" + ss : m + ":" + ss
    }

    focus: true
    onStatusChanged: if (status === PageStatus.Active) forceActiveFocus()
    Keys.onPressed: {
        if (event.key === Qt.Key_Camera || event.key === Qt.Key_Space
                || event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            page.shutter()
            event.accepted = true
        }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    Viewfinder {
        id: vf
        anchors.fill: parent
        // (on X11 a window in the switcher is minimized; the application's
        // state does not become hidden there)
        active: Qt.application.state !== Qt.ApplicationSuspended && Qt.application.state !== Qt.ApplicationHidden
                && QtWindow.Window.visibility !== QtWindow.Window.Minimized
                && QtWindow.Window.visibility !== QtWindow.Window.Hidden
        onCaptured: if (!page.video) flash.restart()
        onFailed: { banner.text = message; banner.show() }
    }

    // the picture taken: a moment of white
    Rectangle {
        id: white
        anchors.fill: parent
        color: "white"
        opacity: 0
        SequentialAnimation on opacity {
            id: flash
            running: false
            NumberAnimation { to: 0.8; duration: 40 }
            NumberAnimation { to: 0; duration: 300 }
        }
    }

    // no camera: the N9's camera in its sleep, and why
    Column {
        anchors.centerIn: parent
        visible: !vf.running && vf.error !== ""
        spacing: 24
        width: parent.width - 260
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            source: "image://theme/icon-l-camera-standby"
        }
        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            text: vf.cameras.length === 0 ? qsTr("No camera") : vf.error
            font.pixelSize: 32
            color: "#8c8c8c"
        }
    }

    // a video's time
    Item {
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top; topMargin: 16 }
        visible: vf.recording
        width: 93; height: 30
        BorderImage {
            anchors.fill: parent
            source: "image://theme/meegotouch-camera-videocapture-timeelapsed-background"
            border { left: 12; right: 12; top: 12; bottom: 12 }
        }
        Row {
            anchors.centerIn: parent
            spacing: 4
            Image {
                anchors.verticalCenter: parent.verticalCenter
                width: 20; height: 20
                source: "image://theme/icon-m-camera-ongoing-recording"
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: page.time(vf.recordSeconds)
                font.pixelSize: 18
                color: "white"
            }
        }
    }

    // the other camera
    Item {
        anchors { left: parent.left; top: parent.top; margins: 16 }
        visible: vf.cameras.length > 1 && !vf.recording
        width: 60; height: 60
        Image {
            anchors.fill: parent
            source: "image://theme/meegotouch-camera-flash-button-background" + (switchArea.pressed ? "-pressed" : "")
        }
        Image {
            anchors.centerIn: parent
            source: "image://theme/icon-m-telephony-videocall-camera-" + (vf.camera === 0 ? "back" : "front")
        }
        MouseArea {
            id: switchArea
            anchors.fill: parent
            onClicked: vf.camera = (vf.camera + 1) % vf.cameras.length
        }
    }

    // the keys at the side
    Item {
        id: side
        anchors { right: parent.right; top: parent.top; bottom: parent.bottom; margins: 16 }
        width: 88
        visible: vf.cameras.length > 0

        // photo or video
        Item {
            anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
            width: 60; height: 120
            opacity: vf.recording ? 0.4 : 1
            Image {
                anchors.fill: parent
                source: "image://theme/meegotouch-camera-capture-mode-switcher-background"
            }
            // both on the track, white; the handle over the chosen one, black
            Image {
                anchors { horizontalCenter: parent.horizontalCenter; top: parent.top; topMargin: 6 }
                source: "image://theme/icon-m-viewfinder-camera"
            }
            Image {
                anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 6 }
                source: "image://theme/icon-m-camera-video"
            }
            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                y: page.video ? 65 : 5
                width: 50; height: 50
                source: "image://theme/meegotouch-camera-capture-mode-switcher-handle-background" + (modeArea.pressed ? "-pressed" : "")
                Behavior on y { NumberAnimation { duration: 150 } }
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/" + (page.video ? "icon-m-camera-video-selected" : "icon-m-viewfinder-camera-selected")
                }
            }
            MouseArea {
                id: modeArea
                anchors.fill: parent
                enabled: !vf.recording && !vf.busy
                onClicked: page.video = !page.video
            }
        }

        // the shutter key
        Item {
            anchors.centerIn: parent
            width: 88; height: 88
            opacity: vf.running ? 1 : 0.4
            Image {
                anchors.fill: parent
                source: "image://theme/meegotouch-camera-shutter-key-background" + (shutterArea.pressed ? "-pressed" : "")
            }
            Image {
                anchors.centerIn: parent
                source: "image://theme/" + (!page.video ? "icon-m-camera-shutter"
                                            : vf.recording ? "icon-m-toolbar-mediacontrol-stop-white"
                                                           : "icon-m-camera-video-record")
            }
            BusyIndicator {
                anchors.centerIn: parent
                visible: vf.busy
                running: visible
                platformStyle: BusyIndicatorStyle { size: "small"; inverted: true }
            }
            MouseArea {
                id: shutterArea
                anchors.fill: parent
                onClicked: page.shutter()
            }
        }

        // the last picture or video
        Item {
            anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom }
            width: 60; height: 60
            visible: vf.last !== ""
            Image {
                anchors.fill: parent
                source: "image://theme/meegotouch-camera-capture-review-button-background" + (reviewArea.pressed ? "-pressed" : "")
            }
            Image {
                anchors { fill: parent; margins: 5 }
                visible: !vf.lastIsVideo
                source: vf.last !== "" && !vf.lastIsVideo ? "file://" + vf.last : ""
                sourceSize.width: 100
                fillMode: Image.PreserveAspectCrop
                clip: true
                asynchronous: true
                cache: false
            }
            Image {
                anchors.centerIn: parent
                visible: vf.lastIsVideo
                source: "image://theme/icon-m-camera-video"
            }
            MouseArea {
                id: reviewArea
                anchors.fill: parent
                onClicked: vf.review()
            }
        }
    }
}
