import QtQuick 2.0
import com.nokia.meego 2.0

// One album's pictures.
Page {
    id: page

    property string title
    property string folder
    property var list: pictures.inFolder(folder)
    Connections { target: pictures; onChanged: page.list = pictures.inFolder(page.folder) }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
    }

    Rectangle { anchors.fill: parent; color: "black" }

    Item {
        id: header
        width: parent.width
        height: 64
        Label {
            anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
            text: page.title + "  (" + page.list.length + ")"
            font.pixelSize: 28
            color: "white"
        }
    }
    Thumbs {
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        items: page.list
        onPicked: window.view(page.list, index)
    }
}
