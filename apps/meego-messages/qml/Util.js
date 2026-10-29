.pragma library

// "14:05" today, "Yesterday", the weekday this week, else the date
function when(d) {
    var now = new Date()
    var day = new Date(d.getFullYear(), d.getMonth(), d.getDate())
    var today = new Date(now.getFullYear(), now.getMonth(), now.getDate())
    var days = Math.round((today - day) / 86400000)
    if (days === 0) return Qt.formatTime(d, Qt.locale().timeFormat(1))
    if (days === 1) return qsTr("Yesterday")
    if (days < 7) return Qt.locale().dayName(d.getDay(), 1)
    return Qt.formatDate(d, Qt.locale().dateFormat(1))
}

function stamp(d) {
    return when(d) === Qt.formatTime(d, Qt.locale().timeFormat(1))
        ? when(d) : when(d) + " " + Qt.formatTime(d, Qt.locale().timeFormat(1))
}
