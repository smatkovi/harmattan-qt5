import QtQuick 2.0
import com.nokia.meego 2.0

// The call history: a tap calls back, a long press offers to put the
// number in the dialler or to delete the call.
Page {
    id: page

    property Item dialer

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon { iconId: "toolbar-dialer"; onClicked: pageStack.pop() }
    }

    // the N9's call history is light
    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    ListView {
        id: list
        anchors.fill: parent
        model: history.calls
        header: Item {
            width: list.width
            height: 80
            Label {
                anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
                text: qsTr("Call history")
                font.pixelSize: 32
                color: "#282828"
            }
        }
        delegate: CallRow {
            call: modelData
            light: true
            onClicked: {
                page.dialer.number = modelData.number
                pageStack.pop()
                page.dialer.call()
            }
            onPressAndHold: {
                menu.index = index
                menu.number = modelData.number
                menu.open()
            }
        }
    }
    ScrollDecorator { flickableItem: list }

    Label {
        anchors.centerIn: parent
        visible: history.calls.length === 0
        text: qsTr("No calls")
        font.pixelSize: 32
        color: "#b4b4b4"
    }

    ContextMenu {
        id: menu
        property int index
        property string number
        MenuLayout {
            MenuItem {
                text: qsTr("Edit number before calling")
                onClicked: { page.dialer.number = menu.number; pageStack.pop() }
            }
            MenuItem { text: qsTr("Delete"); onClicked: history.remove(menu.index) }
        }
    }
}
