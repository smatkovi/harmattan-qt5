import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// One event of a day in a list: its colour stripe, its time, title and
// place.
Item {
    id: item

    property var ev
    property date day
    signal clicked

    width: parent ? parent.width : 0
    height: 88

    Rectangle {
        anchors.fill: parent
        color: area.pressed ? "#d9d9d9" : "transparent"
    }
    Rectangle {
        id: stripe
        x: 16
        width: 8
        anchors { top: parent.top; bottom: parent.bottom; margins: 12 }
        radius: 2
        color: Util.colors[item.ev.color % Util.colors.length]
    }
    Column {
        anchors { left: stripe.right; leftMargin: 16; right: parent.right; rightMargin: 16
                  verticalCenter: parent.verticalCenter }
        Label {
            width: parent.width
            text: item.ev.title !== "" ? item.ev.title : qsTr("(No title)")
            font.pixelSize: 26
            font.bold: true
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            color: "#282828"
        }
        Label {
            width: parent.width
            text: Util.span(item.ev, item.day)
                  + (item.ev.location !== "" ? "  ·  " + item.ev.location : "")
            font.pixelSize: 22
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            color: "#6b6b6b"
        }
    }
    Rectangle {
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
        height: 1
        color: "#d0d0d0"
    }
    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: item.clicked()
    }
}
