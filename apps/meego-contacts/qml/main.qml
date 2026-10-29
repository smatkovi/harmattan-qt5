import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: ContactsPage {}

    function edit(contact) {
        var sheet = Qt.createComponent(Qt.resolvedUrl("ContactSheet.qml")).createObject(pageStack.currentPage)
        sheet.start(contact)
        sheet.open()
    }

    InfoBanner { id: banner; z: 10 }

    // the N9's contacts are light
    Component.onCompleted: theme.inverted = false
}
