import QtQuick 2.0
import com.nokia.meego 2.0

// A setting with its value as a button, opening a selection.
Item {
    id: row

    property alias caption: label.text
    property alias value: button.text
    signal clicked

    width: parent.width
    height: 64

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 26
    }
    Button {
        id: button
        anchors { right: parent.right; verticalCenter: parent.verticalCenter }
        width: 392
        onClicked: row.clicked()
    }
}
