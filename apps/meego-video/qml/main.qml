import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: VideosPage {}

    function play(url, title) {
        pageStack.push(Qt.resolvedUrl("PlayerPage.qml"), { source: url, title: title })
    }

    // the N9's videos are dark
    Component.onCompleted: {
        theme.inverted = true
        if (startVideo !== "")
            play(startVideo, "")
    }
}
