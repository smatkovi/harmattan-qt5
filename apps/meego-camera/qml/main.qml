import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    showToolBar: false
    initialPage: CameraPage {}

    InfoBanner { id: banner; z: 10 }

    // the N9's camera is dark
    Component.onCompleted: theme.inverted = true
}
