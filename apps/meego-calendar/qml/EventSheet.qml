import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// New or changed event: Cancel and Save, the title and place, all day or
// from and to (date and time pickers), the repetition, the reminder, the
// colour and the description.
Sheet {
    id: sheet

    property int eventId: 0
    property date start
    property date end
    property bool allDay: false
    property string recur: "none"
    property var until: undefined
    property int alert: 15
    property int color: 0

    acceptButtonText: qsTr("Save")
    rejectButtonText: qsTr("Cancel")

    function startNew(day) {
        var now = new Date()
        var s = new Date(day.getFullYear(), day.getMonth(), day.getDate(), now.getHours() + 1, 0)
        eventId = 0
        start = s
        end = new Date(s.getTime() + 3600000)
        titleField.text = ""
        placeField.text = ""
        notesArea.text = ""
    }
    function startEdit(ev) {
        eventId = ev.id
        start = ev.start
        // an all-day event ends at the midnight after its last day
        end = ev.allDay ? new Date(ev.end.getTime() - 1000) : ev.end
        allDay = ev.allDay
        recur = ev.recur
        until = ev.until
        alert = ev.alert
        color = ev.color
        titleField.text = ev.title
        placeField.text = ev.location
        notesArea.text = ev.notes
    }

    onAccepted: {
        store.save({ id: eventId, title: titleField.text, location: placeField.text,
                     notes: notesArea.text, start: start, end: end, allDay: allDay,
                     recur: recur, until: until, alert: alert, color: color })
        destroy(1000)
    }
    onRejected: destroy(1000)

    content: Item {
      anchors.fill: parent
      Rectangle { anchors.fill: parent; color: "#f2f2f2" }
      Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: column.height + 32
        clip: true

        Column {
            id: column
            x: 16
            y: 16
            width: parent.width - 32
            spacing: 16

            TextField {
                id: titleField
                width: parent.width
                placeholderText: qsTr("Title")
            }
            TextField {
                id: placeField
                width: parent.width
                placeholderText: qsTr("Location")
            }
            Item {
                width: parent.width
                height: allDaySwitch.height
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("All-day event")
                    font.pixelSize: 26
                }
                Switch {
                    id: allDaySwitch
                    anchors.right: parent.right
                    checked: sheet.allDay
                    onCheckedChanged: sheet.allDay = checked
                }
            }
            DateTimeRow {
                caption: qsTr("Starts")
                value: sheet.start
                showTime: !sheet.allDay
                onDatePicked: {
                    var length = sheet.end.getTime() - sheet.start.getTime()
                    sheet.start = picked
                    sheet.end = new Date(picked.getTime() + length)
                }
                onTimePicked: {
                    var length = sheet.end.getTime() - sheet.start.getTime()
                    sheet.start = picked
                    sheet.end = new Date(picked.getTime() + length)
                }
            }
            DateTimeRow {
                caption: qsTr("Ends")
                value: sheet.end
                showTime: !sheet.allDay
                onDatePicked: sheet.end = picked < sheet.start ? sheet.start : picked
                onTimePicked: sheet.end = picked < sheet.start ? new Date(sheet.start.getTime() + 3600000) : picked
            }
            ValueRow {
                caption: qsTr("Repeat")
                value: Util.recurText(sheet.recur)
                onClicked: recurDialog.open()
            }
            ValueRow {
                caption: qsTr("Alert")
                value: Util.alertText(sheet.alert)
                onClicked: alertDialog.open()
            }
            Item {
                width: parent.width
                height: 64
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Colour")
                    font.pixelSize: 26
                }
                Row {
                    anchors { right: parent.right; verticalCenter: parent.verticalCenter }
                    spacing: 12
                    Repeater {
                        model: Util.colors.length
                        Rectangle {
                            width: 44; height: 44; radius: 22
                            color: Util.colors[index]
                            border.width: sheet.color === index ? 4 : 0
                            border.color: "#282828"
                            MouseArea { anchors.fill: parent; onClicked: sheet.color = index }
                        }
                    }
                }
            }
            TextArea {
                id: notesArea
                width: parent.width
                height: Math.max(160, implicitHeight)
                placeholderText: qsTr("Description")
            }
        }
      }
      ScrollDecorator { flickableItem: flick }
    }

    SelectionDialog {
        id: recurDialog
        titleText: qsTr("Repeat")
        property var keys: ["none", "daily", "weekly", "monthly", "yearly"]
        model: ListModel {
            ListElement { name: "Never" }
            ListElement { name: "Every day" }
            ListElement { name: "Every week" }
            ListElement { name: "Every month" }
            ListElement { name: "Every year" }
        }
        selectedIndex: keys.indexOf(sheet.recur)
        onAccepted: sheet.recur = keys[selectedIndex]
    }
    SelectionDialog {
        id: alertDialog
        titleText: qsTr("Alert")
        property var minutes: [-1, 0, 5, 15, 30, 60, 1440]
        model: ListModel {
            ListElement { name: "None" }
            ListElement { name: "At the start" }
            ListElement { name: "5 minutes before" }
            ListElement { name: "15 minutes before" }
            ListElement { name: "30 minutes before" }
            ListElement { name: "1 hour before" }
            ListElement { name: "1 day before" }
        }
        selectedIndex: minutes.indexOf(sheet.alert)
        onAccepted: sheet.alert = minutes[selectedIndex]
    }
}
