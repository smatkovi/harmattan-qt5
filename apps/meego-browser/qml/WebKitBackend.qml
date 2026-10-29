import QtQuick 2.0
import QtWebKit 3.0 as Kit

// The page on Qt WebKit, as the browser page wants it: the address,
// title, loading and its progress, back and forward.
Item {
    id: root

    readonly property string engine: "Qt WebKit"
    readonly property string url: web.url.toString()
    readonly property string title: web.title
    readonly property bool loading: web.loading
    readonly property real progress: web.loadProgress / 100
    readonly property bool canGoBack: web.canGoBack
    readonly property bool canGoForward: web.canGoForward
    signal loaded(string url, string title)

    function load(u) { web.url = u }
    function back() { web.goBack() }
    function forward() { web.goForward() }
    function reload() { web.reload() }
    function stop() { web.stop() }

    Kit.WebView {
        id: web
        anchors.fill: parent
        onLoadingChanged: {
            if (loadRequest.status === Kit.WebView.LoadSucceededStatus)
                root.loaded(web.url.toString(), web.title)
        }
        onTitleChanged: if (!web.loading && web.title !== "") root.loaded(web.url.toString(), web.title)
    }
}
