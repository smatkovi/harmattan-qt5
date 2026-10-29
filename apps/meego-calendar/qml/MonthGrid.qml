import QtQuick 2.0
import com.nokia.meego 2.0

// A month as the N9 draws it: the month and year with the arrows to the
// months before and after, the weekdays, six weeks of days on the light
// grid; today and the selected day in their cells, a dot under the days
// with events. A sideways swipe changes the month too.
Item {
    id: grid

    property int year
    property int month          // 0-11, as JavaScript's
    property date selected
    property var busy: []       // the days of this month with events
    property bool portrait: width < height * 1.2

    signal picked(date day)
    signal previous
    signal next

    readonly property int firstDay: Qt.locale().firstDayOfWeek  // 0 Sunday .. 6
    readonly property date first: new Date(year, month, 1)
    // the grid's first day: the start of the week that has the 1st
    readonly property int lead: (first.getDay() - firstDay + 7) % 7
    readonly property real cellWidth: width / 7
    readonly property real cellHeight: (height - header.height - weekdays.height) / 6

    function image(name) { return "image://theme/" + name }

    // the month and year, with the arrows
    Item {
        id: header
        width: parent.width
        height: 64
        BorderImage {
            anchors.fill: parent
            source: image("meegotouch-calendar-month-year-area")
            border { left: 3; right: 3; top: 3; bottom: 3 }
        }
        Image {
            id: prevArrow
            anchors { left: parent.left; leftMargin: 24; verticalCenter: parent.verticalCenter }
            source: image("meegotouch-calendar-monthgrid-previousbutton" + (prevArea.pressed ? "-pressed" : ""))
        }
        Image {
            anchors { right: parent.right; rightMargin: 24; verticalCenter: parent.verticalCenter }
            source: image("meegotouch-calendar-monthgrid-nextbutton" + (nextArea.pressed ? "-pressed" : ""))
        }
        Label {
            anchors.centerIn: parent
            text: Qt.formatDate(grid.first, "MMMM yyyy")
            font.pixelSize: 28
            color: "white"
        }
        MouseArea {
            id: prevArea
            width: parent.width / 4; height: parent.height
            onClicked: grid.previous()
        }
        MouseArea {
            id: nextArea
            anchors.right: parent.right
            width: parent.width / 4; height: parent.height
            onClicked: grid.next()
        }
    }

    BorderImage {
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        source: image("meegotouch-calendar-monthgrid-background-" + (grid.portrait ? "portrait" : "landscape"))
        border { left: 10; right: 10; top: 10; bottom: 10 }
    }

    Row {
        id: weekdays
        anchors.top: header.bottom
        height: 36
        Repeater {
            model: 7
            Label {
                width: grid.cellWidth
                height: weekdays.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 18
                color: "#8c8c8c"
                text: Qt.locale().dayName((grid.firstDay + index) % 7, Locale.ShortFormat)
            }
        }
    }

    Grid {
        id: days
        anchors.top: weekdays.bottom
        columns: 7
        Repeater {
            model: 42
            Item {
                id: cell
                readonly property date day: new Date(grid.year, grid.month, 1 - grid.lead + index)
                readonly property bool inMonth: day.getMonth() === grid.month
                readonly property bool today: sameDay(day, new Date())
                readonly property bool isSelected: sameDay(day, grid.selected)
                readonly property bool weekend: day.getDay() === 0 || day.getDay() === 6
                readonly property bool hasEvents: inMonth && grid.busy.indexOf(day.getDate()) >= 0

                function sameDay(a, b) {
                    return a.getFullYear() === b.getFullYear() && a.getMonth() === b.getMonth()
                        && a.getDate() === b.getDate()
                }

                width: grid.cellWidth
                height: grid.cellHeight

                Image {
                    anchors.centerIn: parent
                    visible: cell.today || cell.isSelected
                    source: grid.image("meegotouch-monthgrid-daycell-"
                        + (cell.isSelected ? (cell.today ? "selected-day-current-" : "selected-day-")
                                           : "current-day-")
                        + (grid.portrait ? "portrait" : "landscape"))
                }
                Label {
                    anchors.centerIn: parent
                    text: cell.day.getDate()
                    font.pixelSize: 26
                    font.bold: cell.today
                    color: cell.isSelected ? "white"
                         : !cell.inMonth ? "#c0c0c0"
                         : cell.today ? "#3591e7"
                         : cell.weekend ? "#8c8c8c" : "#282828"
                }
                Image {
                    anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 4 }
                    visible: cell.hasEvents
                    source: grid.image("meegotouch-monthgrid-daycell-"
                        + (cell.isSelected ? "selected-day" : cell.today ? "current-day"
                           : cell.weekend ? "regular-weekend-day" : "regular-day")
                        + "-eventindicator")
                }
            }
        }
    }

    // a sideways swipe over the days changes the month
    MouseArea {
        anchors.fill: days
        propagateComposedEvents: true
        property real startX
        property bool swiped
        onPressed: { startX = mouse.x; swiped = false; mouse.accepted = true }
        onPositionChanged: {
            if (!swiped && Math.abs(mouse.x - startX) > grid.width / 5) {
                swiped = true
                if (mouse.x < startX) grid.next(); else grid.previous()
            }
        }
        onReleased: {
            if (!swiped) {
                // a tap: the day under it
                var col = Math.floor(mouse.x / grid.cellWidth)
                var row = Math.floor(mouse.y / grid.cellHeight)
                grid.picked(new Date(grid.year, grid.month, 1 - grid.lead + row * 7 + col))
            }
        }
    }
}
