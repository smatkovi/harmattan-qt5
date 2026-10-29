import QtQuick 2.0
import com.nokia.meego 2.0
import MeeGo.Video 1.0

// A video full screen: it starts at once; a tap shows the controls for a
// few seconds (the title, the position to drag with the times, back and
// play/pause); the end shows them again.
Page {
    id: page

    property string source
    property string title
    property bool controls: true

    function time(ms) {
        var s = Math.floor(ms / 1000), m = Math.floor(s / 60), h = Math.floor(m / 60)
        var ss = (s % 60 < 10 ? "0" : "") + (s % 60)
        return h > 0 ? h + ":" + (m % 60 < 10 ? "0" : "") + (m % 60) + ":" + ss : m + ":" + ss
    }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: { video.pause(); pageStack.pop() } }
        ToolIcon {
            iconId: video.playing ? "toolbar-mediacontrol-pause" : "toolbar-mediacontrol-play"
            onClicked: { video.toggle(); hide.restart() }
        }
    }
    onControlsChanged: window.showToolBar = controls
    Component.onDestruction: window.showToolBar = true

    Rectangle { anchors.fill: parent; color: "black" }

    Video {
        id: video
        anchors.fill: parent
        source: page.source
        Component.onCompleted: play()
        onEndedChanged: if (ended) { page.controls = true; hide.stop() }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            page.controls = !page.controls
            if (page.controls && video.playing) hide.restart()
        }
    }
    Timer {
        id: hide
        interval: 4000
        running: video.playing && page.controls
        onTriggered: page.controls = false
    }

    Label {
        anchors.centerIn: parent
        visible: video.error !== ""
        width: parent.width - 64
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        text: qsTr("The video does not play: %1").arg(video.error)
        font.pixelSize: 26
        color: "#e4413a"
    }

    // the title above, the position below
    Rectangle {
        visible: page.controls
        width: parent.width
        height: 56
        color: "#aa000000"
        Label {
            anchors { left: parent.left; leftMargin: 16; right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
            text: page.title !== "" ? page.title : decodeURIComponent(page.source.substring(page.source.lastIndexOf("/") + 1))
            font.pixelSize: 24
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            color: "white"
        }
    }
    Rectangle {
        visible: page.controls
        anchors.bottom: parent.bottom
        width: parent.width
        height: 72
        color: "#aa000000"
        Label {
            id: elapsed
            anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
            text: page.time(video.position)
            font.pixelSize: 20
            color: "white"
        }
        Slider {
            id: slider
            anchors { left: elapsed.right; right: total.left; margins: 8; verticalCenter: parent.verticalCenter }
            minimumValue: 0
            maximumValue: Math.max(1, video.duration)
            stepSize: 1000
            Binding on value { when: !slider.pressed; value: video.position }
            onPressedChanged: {
                if (!pressed) video.seek(value)
                hide.restart()
            }
        }
        Label {
            id: total
            anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
            text: page.time(video.duration)
            font.pixelSize: 20
            color: "white"
        }
    }
}
