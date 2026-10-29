import QtQuick 2.0

// An album's cover, or the music icon on a dark tile without one.
Item {
    property string source

    Rectangle {
        anchors.fill: parent
        color: "#262626"
        visible: img.status !== Image.Ready
        Image {
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height) * 0.6
            height: width
            source: "image://theme/icon-l-music"
            opacity: 0.4
            smooth: true
        }
    }
    Image {
        id: img
        anchors.fill: parent
        source: parent.source
        sourceSize.width: 512
        fillMode: Image.PreserveAspectCrop
        clip: true
        asynchronous: true
        smooth: true
    }
}
