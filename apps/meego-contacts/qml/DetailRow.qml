import QtQuick 2.0
import com.nokia.meego 2.0

// A detail of the card: its kind in grey over it; a tap acts on it, the
// icon on the right does a second thing (a message for a number).
Item {
    id: row

    property alias caption: captionLabel.text
    property alias text: valueLabel.text
    property string icon
    signal clicked
    signal iconClicked

    width: parent ? parent.width : 0
    height: Math.max(88, valueLabel.paintedHeight + captionLabel.height + 24)

    Rectangle { anchors.fill: parent; color: area.pressed ? "#d9d9d9" : "transparent" }
    Column {
        anchors { left: parent.left; leftMargin: 16; right: side.left; rightMargin: 16
                  verticalCenter: parent.verticalCenter }
        Label {
            id: captionLabel
            font.pixelSize: 20
            color: "#8c8c8c"
        }
        Label {
            id: valueLabel
            width: parent.width
            font.pixelSize: 26
            wrapMode: Text.Wrap
            color: "#282828"
        }
    }
    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: row.clicked()
    }
    Image {
        id: side
        anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
        visible: row.icon !== ""
        source: row.icon !== "" ? "image://theme/" + row.icon : ""
        width: visible ? implicitWidth : 0
        MouseArea { anchors { fill: parent; margins: -16 } onClicked: row.iconClicked() }
    }
    Rectangle {
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
        height: 1
        color: "#d0d0d0"
    }
}
