import QtQuick 2.0
import com.nokia.meego 2.0

// A key: its sign on black, on a press the N9's yellow square under it
// (the number keys' square, or the function keys' lower one).
Item {
    id: key

    property alias text: label.text
    property bool function_: false
    property color color: function_ ? "#c8c8c8" : "white"
    signal clicked

    BorderImage {
        anchors.centerIn: parent
        width: Math.min(parent.width, 110 * parent.height / (key.function_ ? 90 : 110))
        height: parent.height
        visible: area.pressed
        source: "image://theme/meegoui-calculator-" + (key.function_ ? "function-" : "") + "button-down"
        border { left: 24; right: 24; top: 24; bottom: 24 }
    }
    Label {
        id: label
        anchors.centerIn: parent
        font.pixelSize: Math.min(72, key.height * 0.7)
        font.family: UiConstants.DefaultFontFamilyLight
        color: area.pressed ? "black" : key.color
    }
    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: key.clicked()
    }
}
