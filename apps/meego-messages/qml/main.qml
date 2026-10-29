import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: ConversationsPage {}

    function openConversation(number) {
        pageStack.push(Qt.resolvedUrl("ConversationPage.qml"), { number: number })
    }

    InfoBanner { id: arrived; z: 10 }
    Connections {
        target: sms
        onReceived: {
            if (pageStack.currentPage.number !== number) {
                arrived.text = number + ": " + text
                arrived.show()
            }
        }
    }

    // the N9's conversations are light
    Component.onCompleted: {
        theme.inverted = false
        if (startNumber !== "")
            openConversation(startNumber)
    }
}
