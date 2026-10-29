import QtQuick 2.0
import com.nokia.meego 2.0

// A row of the music lists: an icon or a cover, the title over a grey
// subtitle, a value on the right or the drill-down arrow.
Item {
    id: row

    property string icon
    property string cover
    property bool showCover: false
    property alias title: titleLabel.text
    property alias subtitle: subLabel.text
    property alias value: valueLabel.text
    property bool drilldown: false
    property bool highlighted: false
    signal clicked

    width: parent ? parent.width : 0
    height: 88

    Rectangle {
        anchors.fill: parent
        color: area.pressed ? "#333333" : "transparent"
    }
    Image {
        id: iconImage
        visible: !row.showCover && row.icon !== ""
        anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
        source: row.icon !== "" ? "image://theme/" + row.icon : ""
    }
    Cover {
        id: coverItem
        visible: row.showCover
        anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
        width: 64; height: 64
        source: row.cover
    }
    Column {
        anchors { left: parent.left
                  leftMargin: row.showCover ? 96 : (row.icon !== "" ? 16 + iconImage.width + 16 : 16)
                  right: valueLabel.left; rightMargin: 16; verticalCenter: parent.verticalCenter }
        Label {
            id: titleLabel
            width: parent.width
            font.pixelSize: 26
            font.bold: true
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            color: row.highlighted ? "#ef8b1e" : "white"
        }
        Label {
            id: subLabel
            width: parent.width
            visible: text !== ""
            font.pixelSize: 22
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            color: "#8c8c8c"
        }
    }
    Label {
        id: valueLabel
        anchors { right: arrow.visible ? arrow.left : parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
        font.pixelSize: 22
        color: "#8c8c8c"
    }
    Image {
        id: arrow
        visible: row.drilldown
        anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
        source: "image://theme/icon-m-common-drilldown-arrow-inverse"
    }
    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: row.clicked()
    }
}
