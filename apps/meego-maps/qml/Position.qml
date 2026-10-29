import QtQuick 2.0
import QtPositioning 5.2

// One's position, in a file of its own: without Qt Positioning's QML
// module the map still works, only without it.
PositionSource {
    signal failed

    updateInterval: 1000
    active: false
    onSourceErrorChanged: if (sourceError !== PositionSource.NoError) failed()
}
