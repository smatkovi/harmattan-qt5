import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The events of the coming weeks, by day.
Page {
    id: page

    property var events: store.upcoming(60)
    Connections { target: store; onChanged: page.events = store.upcoming(60) }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    ListView {
        id: list
        anchors.fill: parent
        model: page.events
        delegate: Column {
            width: list.width
            // the day above its first event
            Label {
                visible: index === 0 || !Util.sameDay(page.events[index - 1].day, modelData.day)
                x: 16
                height: visible ? 48 : 0
                verticalAlignment: Text.AlignBottom
                text: Util.longDate(modelData.day)
                font.pixelSize: 22
                color: "#3591e7"
            }
            EventDelegate {
                ev: modelData
                day: modelData.day
                onClicked: pageStack.push(Qt.resolvedUrl("EventPage.qml"), { eventId: modelData.id, day: modelData.day })
            }
        }
        header: Label {
            x: 16
            height: 64
            verticalAlignment: Text.AlignVCenter
            text: qsTr("Upcoming events")
            font.pixelSize: 32
            color: "#282828"
        }
    }
    ScrollDecorator { flickableItem: list }
    Label {
        anchors.centerIn: parent
        visible: page.events.length === 0
        text: qsTr("No upcoming events")
        font.pixelSize: 32
        color: "#b4b4b4"
    }
}
