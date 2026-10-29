import QtQuick 2.0
import QtWebEngine 1.10 as Engine

// The page on Qt WebEngine (Chromium), as the browser page wants it: the
// address, title, loading and its progress, back and forward; a page's
// new window opens in this one; no position, camera or microphone.
Item {
    id: root

    readonly property string engine: "Qt WebEngine"
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

    Engine.WebEngineView {
        id: web
        anchors.fill: parent
        onLoadingChanged: {
            if (loadRequest.status === Engine.WebEngineView.LoadSucceededStatus)
                root.loaded(web.url.toString(), web.title)
        }
        onTitleChanged: if (!web.loading && web.title !== "") root.loaded(web.url.toString(), web.title)
        onNewViewRequested: web.url = request.requestedUrl
        onFeaturePermissionRequested: grantFeaturePermission(securityOrigin, feature, false)
    }
}
