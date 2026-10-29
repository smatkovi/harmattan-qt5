import QtQuick 2.0

// A contact's photo, or the N9's placeholder.
Item {
    property string source
    property string placeholder: "icon-m-content-avatar-placeholder"

    Image {
        anchors.fill: parent
        visible: photo.status !== Image.Ready
        source: "image://theme/" + parent.placeholder
        smooth: true
    }
    Image {
        id: photo
        anchors.fill: parent
        source: parent.source
        fillMode: Image.PreserveAspectCrop
        clip: true
        smooth: true
        asynchronous: true
    }
}
