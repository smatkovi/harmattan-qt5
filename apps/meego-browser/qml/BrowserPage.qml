import QtQuick 2.5
import com.nokia.meego 2.0

// The browser: the address field above (the page's address; Enter opens
// what is written, an address or else a search; stop while loading, the
// load's progress under it), the page or the start view with the
// bookmarks and the history; below back, forward, the start, the
// bookmark and the menu. The web view is Qt WebEngine's or Qt WebKit's,
// whichever is installed; with neither the start view stays and says so.
Page {
    id: page

    property Item view: null
    property string engineError: ""
    property bool showStart: true
    readonly property string current: view && !showStart ? view.url : ""
    property bool bookmarked: false
    function updateBookmarked() { bookmarked = current !== "" && store.isBookmarked(current) }
    onCurrentChanged: updateBookmarked()
    Connections { target: store; onChanged: page.updateBookmarked() }

    function open(text) {
        var u = store.fixUrl(text)
        if (u === "")
            return
        address.focus = false
        page.forceActiveFocus()
        if (!view) {
            showStart = true
            banner.text = qsTr("No web engine: Qt WebEngine or Qt WebKit for QML is not installed")
            banner.show()
            return
        }
        showStart = false
        view.load(u)
    }

    // the first web view that loads
    Component.onCompleted: {
        var errors = []
        for (var i = 0; i < webViews.length && !view; ++i) {
            var c = Qt.createComponent(webViews[i])
            if (c.status === Component.Ready)
                view = c.createObject(content, { "anchors.fill": content })
            else
                errors.push(c.errorString())
        }
        if (!view) {
            engineError = errors.join("\n")
            console.log("meego-browser: no web view:\n" + engineError)
        } else {
            view.loaded.connect(function (u, t) { store.visit(u, t) })
        }
        if (startUrl !== "")
            open(startUrl)
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            enabled: !page.showStart || (page.view && page.view.url !== "")
            opacity: enabled ? 1 : 0.4
            onClicked: {
                if (page.showStart) page.showStart = false
                else if (page.view.canGoBack) page.view.back()
                else page.showStart = true
            }
        }
        ToolIcon {
            iconId: "toolbar-next"
            enabled: page.view !== null && !page.showStart && page.view.canGoForward
            opacity: enabled ? 1 : 0.4
            onClicked: page.view.forward()
        }
        ToolIcon {
            iconId: "toolbar-home"
            onClicked: page.showStart = true
        }
        ToolIcon {
            iconId: page.bookmarked ? "toolbar-favorite-mark" : "toolbar-favorite-unmark"
            enabled: page.current !== ""
            opacity: enabled ? 1 : 0.4
            onClicked: {
                store.toggleBookmark(page.current, page.view.title)
                banner.text = page.bookmarked ? qsTr("Added to bookmarks") : qsTr("Removed from bookmarks")
                banner.show()
            }
        }
        ToolIcon { iconId: "toolbar-view-menu"; onClicked: menu.open() }
    }

    Menu {
        id: menu
        MenuLayout {
            MenuItem {
                text: qsTr("Reload")
                enabled: page.current !== ""
                onClicked: page.view.reload()
            }
            MenuItem {
                text: qsTr("Clear history")
                onClicked: store.clearHistory()
            }
        }
    }

    Rectangle { anchors.fill: parent; color: "white" }

    // the address field
    Rectangle {
        id: bar
        anchors { left: parent.left; right: parent.right; top: parent.top }
        height: 72
        color: "#e6e6e6"
        z: 1
        TextField {
            id: address
            anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; margins: 12 }
            placeholderText: qsTr("Address or search")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            platformStyle: TextFieldStyle { paddingRight: 56 }
            text: page.current
            onActiveFocusChanged: if (activeFocus) selectAll(); else text = Qt.binding(function () { return page.current })
            Keys.onReturnPressed: page.open(text)
            Keys.onEnterPressed: page.open(text)
            Image {
                anchors { right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
                visible: (page.view && page.view.loading && !page.showStart) || (address.activeFocus && address.text !== "")
                source: "image://theme/" + (page.view && page.view.loading && !page.showStart ? "icon-m-browser-stop" : "icon-m-browser-clear")
                MouseArea {
                    anchors { fill: parent; margins: -8 }
                    onClicked: {
                        if (page.view && page.view.loading && !page.showStart) page.view.stop()
                        else address.text = ""
                    }
                }
            }
        }
        // the load's progress
        Rectangle {
            anchors { left: parent.left; bottom: parent.bottom }
            height: 4
            width: parent.width * (page.view ? page.view.progress : 0)
            visible: page.view !== null && page.view.loading && !page.showStart
            color: "#3591e7"
        }
    }

    Item {
        id: content
        anchors { left: parent.left; right: parent.right; top: bar.bottom; bottom: parent.bottom }
        visible: !page.showStart
    }

    StartView {
        anchors { left: parent.left; right: parent.right; top: bar.bottom; bottom: parent.bottom }
        visible: page.showStart
        onOpen: page.open(url)
    }

    // no web engine: said once at the top of the start view
    Rectangle {
        anchors { left: parent.left; right: parent.right; top: bar.bottom }
        visible: page.view === null && page.showStart
        height: noEngine.height + 24
        color: "#fff4d6"
        Label {
            id: noEngine
            anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; margins: 16 }
            text: qsTr("No web engine: install Qt WebEngine or Qt WebKit for QML to see pages.")
            wrapMode: Text.Wrap
            font.pixelSize: 22
            color: "#6e5400"
        }
    }

    Shortcut { sequence: "Ctrl+L"; onActivated: { address.forceActiveFocus(); address.selectAll() } }
    Shortcut { sequence: "F5"; enabled: page.current !== ""; onActivated: page.view.reload() }
    Shortcut { sequence: "Alt+Left"; enabled: page.view !== null && page.view.canGoBack; onActivated: page.view.back() }
    Shortcut { sequence: "Alt+Right"; enabled: page.view !== null && page.view.canGoForward; onActivated: page.view.forward() }
}
