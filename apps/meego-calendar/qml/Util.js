.pragma library

// Times and dates in the locale's short form, as the N9's calendar shows
// them.

function time(d) {
    return Qt.formatTime(d, Qt.locale().timeFormat(1 /* ShortFormat */))
}

function sameDay(a, b) {
    return a.getFullYear() === b.getFullYear() && a.getMonth() === b.getMonth()
        && a.getDate() === b.getDate()
}

function dayOnly(d) {
    return new Date(d.getFullYear(), d.getMonth(), d.getDate())
}

// "All day", "09:00 – 10:30", or the days when an event spans several
function span(ev, day) {
    if (ev.allDay)
        return qsTr("All day")
    var s = ev.start, e = ev.end
    if (day && !sameDay(s, e)) {
        if (sameDay(s, day))
            return time(s) + " –"
        if (sameDay(e, day))
            return "– " + time(e)
        return qsTr("All day")
    }
    return time(s) + " – " + time(e)
}

function longDate(d) {
    return Qt.formatDate(d, Qt.locale().dateFormat(0 /* LongFormat */))
}

function recurText(r) {
    switch (r) {
    case "daily": return qsTr("Every day")
    case "weekly": return qsTr("Every week")
    case "monthly": return qsTr("Every month")
    case "yearly": return qsTr("Every year")
    }
    return qsTr("Never")
}

function alertText(minutes) {
    if (minutes < 0) return qsTr("None")
    if (minutes === 0) return qsTr("At the start")
    if (minutes < 60) return qsTr("%1 minutes before").arg(minutes)
    if (minutes < 1440)
        return minutes === 60 ? qsTr("1 hour before") : qsTr("%1 hours before").arg(minutes / 60)
    return minutes === 1440 ? qsTr("1 day before") : qsTr("%1 days before").arg(minutes / 1440)
}

// the calendar colours of the N9 (its default calendar is blue)
var colors = ["#3591e7", "#5bc23a", "#ef8b1e", "#d9281f", "#a14cd8", "#2fb8b4"]
