import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: InboxPage {}

    function compose(fields) {
        pageStack.push(Qt.resolvedUrl("ComposePage.qml"), fields || {})
    }
    function setupAccount() {
        var sheet = Qt.createComponent(Qt.resolvedUrl("AccountSheet.qml")).createObject(pageStack.currentPage)
        sheet.open()
    }

    InfoBanner { id: banner; z: 10 }
    Connections {
        target: mail
        onFailed: { banner.text = error; banner.show() }
        onSent: { banner.text = qsTr("Message sent"); banner.show() }
    }

    // the N9's mail is light
    Component.onCompleted: {
        theme.inverted = false
        if (!mail.configured)
            setupAccount()
        else
            mail.refresh()
        if (startTo !== "")
            compose({ to: startTo })
    }
}
