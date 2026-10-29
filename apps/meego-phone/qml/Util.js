.pragma library

function duration(s) {
    s = Math.max(0, Math.floor(s))
    var m = Math.floor(s / 60), h = Math.floor(m / 60)
    var ss = (s % 60 < 10 ? "0" : "") + (s % 60)
    if (h > 0)
        return h + ":" + (m % 60 < 10 ? "0" : "") + (m % 60) + ":" + ss
    return m + ":" + ss
}

// "14:05" today, "Yesterday 14:05", else the date
function when(d) {
    var now = new Date()
    var day = new Date(d.getFullYear(), d.getMonth(), d.getDate())
    var today = new Date(now.getFullYear(), now.getMonth(), now.getDate())
    var t = Qt.formatTime(d, Qt.locale().timeFormat(1))
    var days = Math.round((today - day) / 86400000)
    if (days === 0) return t
    if (days === 1) return qsTr("Yesterday") + " " + t
    if (days < 7) return Qt.locale().dayName(d.getDay(), 1) + " " + t
    return Qt.formatDate(d, Qt.locale().dateFormat(1))
}

// the digits as the N9 groups them while dialling: as typed
function display(number) {
    return number
}

var letters = { "1": "", "2": "abc", "3": "def", "4": "ghi", "5": "jkl", "6": "mno",
                "7": "pqrs", "8": "tuv", "9": "wxyz", "*": "", "0": "+", "#": "" }
