import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// A call of the history: received, dialled or missed, the number, when
// and how long. On the light history the N9's icons; on the dark dialler
// (whose icons those are not) coloured arrows.
Item {
    id: row

    property var call
    property bool light: false
    signal clicked
    signal pressAndHold

    width: parent ? parent.width : 0
    height: 88

    Rectangle { anchors.fill: parent; color: area.pressed ? (row.light ? "#d9d9d9" : "#333333") : "transparent" }
    Item {
        id: kind
        anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
        width: 32; height: 32
        Image {
            anchors.centerIn: parent
            visible: row.light
            source: visible ? "image://theme/icon-m-telephony-callhistory-" + row.call.kind : ""
        }
        Label {
            anchors.centerIn: parent
            visible: !row.light
            text: row.call.kind === "initiated" ? "↗" : "↙"
            font.pixelSize: 30
            font.bold: true
            color: row.call.kind === "missed" ? "#e4413a" : row.call.kind === "received" ? "#5bc23a" : "#3591e7"
        }
    }
    Column {
        anchors { left: kind.right; leftMargin: 16; right: parent.right; rightMargin: 16
                  verticalCenter: parent.verticalCenter }
        Label {
            width: parent.width
            text: row.call.number !== "" ? row.call.number : qsTr("Unknown number")
            font.pixelSize: 26
            font.bold: true
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            color: row.call.kind === "missed" ? "#e4413a" : row.light ? "#282828" : "white"
        }
        Label {
            width: parent.width
            text: Util.when(row.call.start)
                  + (row.call.seconds > 0 ? "  ·  " + Util.duration(row.call.seconds) : "")
            font.pixelSize: 22
            color: row.light ? "#6b6b6b" : "#8c8c8c"
        }
    }
    Rectangle {
        visible: row.light
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: 16; rightMargin: 16 }
        height: 1
        color: "#d0d0d0"
    }
    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: row.clicked()
        onPressAndHold: row.pressAndHold()
    }
}
