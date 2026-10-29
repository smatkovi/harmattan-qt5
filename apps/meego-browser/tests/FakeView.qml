import QtQuick 2.0

// A stand-in for the web view in tests (MEEGO_BROWSER_VIEW=file:///…/FakeView.qml):
// local HTML files as Qt's rich text, links followed, back and forward, a
// load that takes a moment.
Item {
    id: root

    readonly property string engine: "test"
    property string url: ""
    property string title: ""
    property bool loading: false
    property real progress: 0
    property var past: []
    property var ahead: []
    readonly property bool canGoBack: past.length > 0
    readonly property bool canGoForward: ahead.length > 0
    signal loaded(string url, string title)

    function show(u) {
        url = u
        loading = true
        progress = 0.1
        timer.restart()
        var x = new XMLHttpRequest()
        x.onreadystatechange = function () {
            if (x.readyState !== XMLHttpRequest.DONE)
                return
            var html = x.responseText || "<h1>Not found</h1><p>" + u + "</p>"
            var m = /<title>([^<]*)<\/title>/i.exec(html)
            text.text = html
            title = m ? m[1] : u
        }
        x.open("GET", u)
        x.send()
    }
    function load(u) { if (url !== "") past = past.concat([url]); ahead = []; show(u) }
    function back() { var p = past.slice(); ahead = [url].concat(ahead); show(p.pop()); past = p }
    function forward() { var a = ahead.slice(); past = past.concat([url]); show(a.shift()); ahead = a }
    function reload() { show(url) }
    function stop() { timer.stop(); loading = false }

    Timer {
        id: timer
        interval: 300
        repeat: true
        onTriggered: {
            root.progress += 0.3
            if (root.progress >= 1) {
                stop()
                root.loading = false
                root.loaded(root.url, root.title)
            }
        }
    }
    Flickable {
        anchors.fill: parent
        contentHeight: text.height + 32
        clip: true
        Text {
            id: text
            x: 16; y: 16
            width: parent.width - 32
            textFormat: Text.RichText
            wrapMode: Text.Wrap
            font.pixelSize: 22
            onLinkActivated: root.load(/^[a-z]+:/.test(link) ? link : root.url.replace(/[^\/]*$/, "") + link)
        }
    }
}
