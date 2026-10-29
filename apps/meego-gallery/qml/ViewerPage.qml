import QtQuick 2.0
import com.nokia.meego 2.0

// A picture full screen: a sideways swipe to the next or the one before,
// a double tap zooms in and out (drag the zoomed picture), a tap shows or
// hides the tool bar: back, details, delete.
Page {
    id: page

    property var list: []
    property int start: 0
    property bool chrome: true
    readonly property var current: list.length > 0 ? list[view.currentIndex] : null

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: details.open() }
        ToolIcon { iconId: "toolbar-delete"; onClicked: confirm.open() }
    }
    Component.onCompleted: view.positionViewAtIndex(start, ListView.Beginning)
    onChromeChanged: window.showToolBar = chrome
    Component.onDestruction: window.showToolBar = true

    Rectangle { anchors.fill: parent; color: "black" }

    ListView {
        id: view
        anchors.fill: parent
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        boundsBehavior: Flickable.StopAtBounds
        interactive: currentItem ? !currentItem.zoomed : true
        model: page.list
        cacheBuffer: width
        delegate: Flickable {
            id: flick
            property bool zoomed: scale > 1.01
            property real scale: 1
            width: view.width
            height: view.height
            clip: true
            contentWidth: width * scale
            contentHeight: height * scale
            interactive: zoomed
            Image {
                id: image
                width: flick.contentWidth
                height: flick.contentHeight
                source: modelData.url
                // the screen's size, not the camera's (memory)
                sourceSize.width: Math.min(2048, view.width * 2)
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                smooth: true
            }
            BusyIndicator {
                anchors.centerIn: parent
                running: image.status === Image.Loading
                visible: running
            }
            MouseArea {
                anchors.fill: parent
                onClicked: tapTimer.restart()
                onDoubleClicked: {
                    tapTimer.stop()
                    if (flick.zoomed) {
                        flick.scale = 1
                    } else {
                        var x = mouse.x, y = mouse.y
                        flick.scale = 2.5
                        flick.contentX = Math.max(0, x * 2.5 - flick.width / 2)
                        flick.contentY = Math.max(0, y * 2.5 - flick.height / 2)
                    }
                }
                Timer { id: tapTimer; interval: 300; onTriggered: page.chrome = !page.chrome }
            }
        }
    }

    // the name and the place in the list, with the tool bar
    Rectangle {
        visible: page.chrome && page.current !== null
        width: parent.width
        height: 56
        color: "#aa000000"
        Label {
            anchors { left: parent.left; leftMargin: 16; right: count.left; rightMargin: 16; verticalCenter: parent.verticalCenter }
            text: page.current ? page.current.name : ""
            font.pixelSize: 22
            elide: Text.ElideMiddle
            wrapMode: Text.NoWrap
            color: "white"
        }
        Label {
            id: count
            anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
            text: (view.currentIndex + 1) + " / " + page.list.length
            font.pixelSize: 22
            color: "#c8c8c8"
        }
    }

    QueryDialog {
        id: details
        titleText: page.current ? page.current.name : ""
        message: {
            if (!page.current)
                return ""
            var s = pictures.dimensions(page.current.path)
            return Qt.formatDateTime(page.current.time, Qt.locale().dateTimeFormat(1)) + "\n"
                 + s.width + " × " + s.height + "\n"
                 + Math.round(page.current.size / 1024) + " kB\n" + page.current.folder
        }
        acceptButtonText: qsTr("Close")
    }
    QueryDialog {
        id: confirm
        titleText: qsTr("Delete picture?")
        message: page.current ? page.current.name : ""
        acceptButtonText: qsTr("Delete")
        rejectButtonText: qsTr("Cancel")
        onAccepted: {
            var i = view.currentIndex
            if (pictures.remove(page.current.path)) {
                var l = page.list.slice()
                l.splice(i, 1)
                page.list = l
                if (l.length === 0)
                    pageStack.pop()
                else
                    view.currentIndex = Math.min(i, l.length - 1)
            }
        }
    }
}
