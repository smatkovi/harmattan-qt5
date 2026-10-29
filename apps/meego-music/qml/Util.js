.pragma library

function time(ms) {
    if (!ms || ms < 0) ms = 0
    var s = Math.floor(ms / 1000)
    var m = Math.floor(s / 60)
    s = s % 60
    var h = Math.floor(m / 60)
    if (h > 0)
        return h + ":" + (m % 60 < 10 ? "0" : "") + (m % 60) + ":" + (s < 10 ? "0" : "") + s
    return m + ":" + (s < 10 ? "0" : "") + s
}

function artistOf(song) {
    return song.artist !== "" ? song.artist : qsTr("Unknown artist")
}

function albumOf(song) {
    return song.album !== "" ? song.album : qsTr("Unknown album")
}

function count(n, one, many) {
    return n === 1 ? one : many.arg(n)
}
