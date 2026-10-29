import QtQuick 2.0
import com.nokia.meego 2.0

PageStackWindow {
    id: window

    // Hildon and the MeeGo session have their own status bars
    showStatusBar: false
    initialPage: MonthPage {}

    Component.onCompleted: theme.inverted = false
}
