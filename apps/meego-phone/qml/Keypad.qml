import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The N9's keypad: the digits with their letters on the dialler's keys,
// a long press on 0 gives "+".
Grid {
    id: pad

    property real keyWidth: width / 3
    property real keyHeight: height / 4
    signal key(string k)

    columns: 3

    Repeater {
        model: ["1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"]
        Item {
            width: pad.keyWidth
            height: pad.keyHeight
            BorderImage {
                anchors { fill: parent; margins: 4 }
                source: "image://theme/meegotouch-callui-dialler-button-background" + (area.pressed ? "-pressed" : "")
                border { left: 16; right: 16; top: 16; bottom: 16 }
            }
            Row {
                anchors.centerIn: parent
                spacing: 8
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData
                    font.pixelSize: Math.min(56, pad.keyHeight * 0.6)
                    color: "white"
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 48
                    text: Util.letters[modelData].toUpperCase()
                    font.pixelSize: 18
                    color: "#8c8c8c"
                }
            }
            MouseArea {
                id: area
                anchors.fill: parent
                onClicked: pad.key(modelData)
                onPressAndHold: pad.key(modelData === "0" ? "+" : modelData)
            }
        }
    }
}
