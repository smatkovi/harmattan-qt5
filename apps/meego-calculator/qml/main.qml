import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    showStatusBar: false
    showToolBar: false
    initialPage: CalculatorPage {}

    InfoBanner { id: banner; z: 10 }

    // the N9's calculator is dark
    Component.onCompleted: theme.inverted = true
}
