import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    initialPage: MapPage { id: mapPage }

    InfoBanner { id: banner; z: 10 }

    // the N9's maps are light
    Component.onCompleted: theme.inverted = false
}
