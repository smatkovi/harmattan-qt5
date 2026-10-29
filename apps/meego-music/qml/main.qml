import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: MainPage {}

    function nowPlaying() {
        if (pageStack.currentPage.objectName !== "nowPlaying")
            pageStack.push(Qt.resolvedUrl("NowPlayingPage.qml"))
    }

    // the N9's music player is dark
    Component.onCompleted: theme.inverted = true
}
