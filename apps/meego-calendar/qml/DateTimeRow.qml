import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// "Starts" or "Ends": the date and the time as buttons, each opening its
// picker.
Item {
    id: row

    property alias caption: label.text
    property date value
    property bool showTime: true
    signal datePicked(date picked)
    signal timePicked(date picked)

    width: parent.width
    height: 64

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 26
    }
    Row {
        anchors { right: parent.right; verticalCenter: parent.verticalCenter }
        spacing: 12
        Button {
            width: 240
            text: Qt.formatDate(row.value, Qt.locale().dateFormat(1 /* ShortFormat */))
            onClicked: {
                datePicker.year = row.value.getFullYear()
                datePicker.month = row.value.getMonth() + 1
                datePicker.day = row.value.getDate()
                datePicker.open()
            }
        }
        Button {
            visible: row.showTime
            width: 140
            text: Util.time(row.value)
            onClicked: {
                timePicker.hour = row.value.getHours()
                timePicker.minute = row.value.getMinutes()
                timePicker.open()
            }
        }
    }

    DatePickerDialog {
        id: datePicker
        titleText: row.caption
        acceptButtonText: qsTr("Done")
        rejectButtonText: qsTr("Cancel")
        minimumYear: 1970
        maximumYear: 2100
        onAccepted: row.datePicked(new Date(year, month - 1, day,
                                            row.value.getHours(), row.value.getMinutes()))
    }
    TimePickerDialog {
        id: timePicker
        titleText: row.caption
        acceptButtonText: qsTr("Done")
        rejectButtonText: qsTr("Cancel")
        fields: DateTime.Hours | DateTime.Minutes
        onAccepted: row.timePicked(new Date(row.value.getFullYear(), row.value.getMonth(),
                                            row.value.getDate(), hour, minute))
    }
}
