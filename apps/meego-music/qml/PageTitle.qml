import QtQuick 2.0
import com.nokia.meego 2.0

// The title above a list.
Item {
    property alias text: label.text

    width: parent ? parent.width : 0
    height: 80
    Label {
        id: label
        anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
        font.pixelSize: 32
        color: "white"
    }
}
