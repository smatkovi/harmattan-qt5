import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: NotesPage {}

    // a note in the editor sheet; none: a new one
    function edit(id) {
        var sheet = Qt.createComponent(Qt.resolvedUrl("NoteSheet.qml")).createObject(pageStack.currentPage)
        sheet.start(id || "")
        sheet.open()
    }

    InfoBanner { id: banner; z: 10 }

    // the N9's notes are light
    Component.onCompleted: {
        theme.inverted = false
        if (startNew)
            edit("")
    }
}
