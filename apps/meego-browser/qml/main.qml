import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: BrowserPage {}

    InfoBanner { id: banner; z: 10 }

    // the N9's browser is light
    Component.onCompleted: theme.inverted = false
}
