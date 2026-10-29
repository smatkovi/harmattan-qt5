import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// The calendar's main view: the month, and the events of the selected
// day below it (portrait) or beside it (landscape).
Page {
    id: page

    property date selected: Util.dayOnly(new Date())
    property int year: selected.getFullYear()
    property int month: selected.getMonth()
    property var dayEvents: []
    property var busy: []

    function reload() {
        busy = store.busyDays(year, month + 1)
        dayEvents = store.eventsOn(selected)
    }
    function showMonth(y, m) {
        var d = new Date(y, m, 1)
        year = d.getFullYear()
        month = d.getMonth()
        // the selected day follows to the same day of the new month
        var last = new Date(year, month + 1, 0).getDate()
        selected = new Date(year, month, Math.min(selected.getDate(), last))
        reload()
    }
    function newEvent() {
        var sheet = sheetComponent.createObject(page)
        sheet.startNew(selected)
        sheet.open()
    }

    Component.onCompleted: reload()
    Connections { target: store; onChanged: page.reload() }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-jump-to"
            onClicked: {
                page.selected = Util.dayOnly(new Date())
                page.showMonth(page.selected.getFullYear(), page.selected.getMonth())
            }
        }
        ToolIcon { iconId: "toolbar-add"; onClicked: page.newEvent() }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: menu.open() }
    }

    Menu {
        id: menu
        MenuLayout {
            MenuItem {
                text: qsTr("Go to date")
                onClicked: {
                    goTo.year = page.selected.getFullYear()
                    goTo.month = page.selected.getMonth() + 1
                    goTo.day = page.selected.getDate()
                    goTo.open()
                }
            }
            MenuItem {
                text: qsTr("Upcoming events")
                onClicked: pageStack.push(Qt.resolvedUrl("AgendaPage.qml"))
            }
        }
    }

    DatePickerDialog {
        id: goTo
        titleText: qsTr("Go to date")
        acceptButtonText: qsTr("Go")
        rejectButtonText: qsTr("Cancel")
        minimumYear: 1970
        maximumYear: 2100
        onAccepted: {
            page.selected = new Date(year, month - 1, day)
            page.showMonth(year, month - 1)
        }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    MonthGrid {
        id: grid
        anchors { left: parent.left; top: parent.top }
        width: page.width > page.height ? page.width * 0.55 : page.width
        height: page.width > page.height ? page.height : Math.min(page.height * 0.62, 456 + 64)
        year: page.year
        month: page.month
        selected: page.selected
        busy: page.busy
        onPicked: {
            if (day.getMonth() !== page.month) {
                page.selected = day
                page.showMonth(day.getFullYear(), day.getMonth())
            } else {
                page.selected = day
                page.dayEvents = store.eventsOn(day)
            }
        }
        onPrevious: page.showMonth(page.year, page.month - 1)
        onNext: page.showMonth(page.year, page.month + 1)
    }

    // the selected day's events
    Item {
        id: dayArea
        anchors { left: page.width > page.height ? grid.right : parent.left
                  top: page.width > page.height ? parent.top : grid.bottom
                  right: parent.right; bottom: parent.bottom }

        Label {
            id: dayTitle
            anchors { left: parent.left; leftMargin: 16; top: parent.top; topMargin: 12 }
            text: Util.longDate(page.selected)
            font.pixelSize: 22
            color: "#6b6b6b"
        }
        ListView {
            id: list
            anchors { top: dayTitle.bottom; topMargin: 4; left: parent.left; right: parent.right; bottom: parent.bottom }
            clip: true
            model: page.dayEvents
            delegate: EventDelegate {
                ev: modelData
                day: page.selected
                onClicked: pageStack.push(Qt.resolvedUrl("EventPage.qml"), { eventId: modelData.id, day: page.selected })
            }
        }
        ScrollDecorator { flickableItem: list }
        Label {
            anchors.centerIn: list
            visible: page.dayEvents.length === 0
            text: qsTr("No events")
            font.pixelSize: 32
            color: "#b4b4b4"
            MouseArea {
                anchors.fill: parent
                anchors.margins: -40
                onClicked: page.newEvent()
            }
        }
    }

    Component {
        id: sheetComponent
        EventSheet {}
    }
}
