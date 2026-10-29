.pragma library

function when(d) {
    if (!d || isNaN(d.getTime()))
        return ""
    var now = new Date()
    var day = new Date(d.getFullYear(), d.getMonth(), d.getDate())
    var today = new Date(now.getFullYear(), now.getMonth(), now.getDate())
    var days = Math.round((today - day) / 86400000)
    if (days === 0) return Qt.formatTime(d, Qt.locale().timeFormat(1))
    if (days === 1) return qsTr("Yesterday")
    if (days < 7) return Qt.locale().dayName(d.getDay(), 1)
    return Qt.formatDate(d, Qt.locale().dateFormat(1))
}

function full(d) {
    if (!d || isNaN(d.getTime()))
        return ""
    return Qt.formatDateTime(d, Qt.locale().dateTimeFormat(1))
}

// "Name <a@b>" -> "Name"
function name(from) {
    var lt = from.indexOf("<")
    var n = lt > 0 ? from.substring(0, lt).trim() : from
    return n.replace(/^"|"$/g, "")
}

function quote(text) {
    return text.split("\n").map(function (l) { return "> " + l }).join("\n")
}

function prefixed(prefix, subject) {
    return subject.toLowerCase().indexOf(prefix.toLowerCase()) === 0 ? subject : prefix + " " + subject
}
