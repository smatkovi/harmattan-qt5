import QtQuick 2.0
import com.nokia.meego 2.0

// A caption in grey over its value, as in the N9's event details.
Column {
    property alias caption: captionLabel.text
    property alias text: valueLabel.text

    width: parent.width
    spacing: 2
    Label {
        id: captionLabel
        width: parent.width
        font.pixelSize: 20
        color: "#8c8c8c"
    }
    Label {
        id: valueLabel
        width: parent.width
        font.pixelSize: 26
        color: "#282828"
        wrapMode: Text.Wrap
    }
}
