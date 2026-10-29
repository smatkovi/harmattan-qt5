import QtQuick 2.0
import com.nokia.meego 2.0

// A round call button of the N9's call screen with its name below.
Column {
    id: button

    property string background
    property string icon
    property alias text: label.text
    property bool enabled: true
    signal clicked

    width: 120
    spacing: 8
    opacity: enabled ? 1 : 0.5

    BorderImage {
        anchors.horizontalCenter: parent.horizontalCenter
        width: 96; height: 96
        source: "image://theme/" + button.background + (area.pressed ? "-pressed" : "")
        border { left: 24; right: 24; top: 24; bottom: 24 }
        Image {
            anchors.centerIn: parent
            source: "image://theme/" + button.icon
        }
        MouseArea {
            id: area
            anchors.fill: parent
            enabled: button.enabled
            onClicked: button.clicked()
        }
    }
    Label {
        id: label
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 20
        color: "#c8c8c8"
    }
}
