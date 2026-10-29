import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: DialerPage { id: dialer }

    // the call screen comes up for every call, dialled or coming in
    Connections {
        target: modem
        onInCallChanged: {
            if (modem.inCall && pageStack.currentPage.objectName !== "call")
                pageStack.push(Qt.resolvedUrl("CallPage.qml"))
        }
    }
    Component.onCompleted: {
        theme.inverted = true
        if (modem.inCall)
            pageStack.push(Qt.resolvedUrl("CallPage.qml"))
    }
}
