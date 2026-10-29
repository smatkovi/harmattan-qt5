import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: GridPage {}

    function view(list, index) {
        pageStack.push(Qt.resolvedUrl("ViewerPage.qml"), { list: list, start: index })
    }

    // a picture given at the start: full screen once the pictures are found
    property bool opened: startPicture === ""
    Connections {
        target: pictures
        onChanged: {
            if (!window.opened) {
                window.opened = true
                var i = pictures.indexOf(startPicture)
                if (i >= 0)
                    window.view(pictures.all, i)
            }
        }
    }

    // the N9's gallery is dark
    Component.onCompleted: theme.inverted = true
}
