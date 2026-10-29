import QtQuick 2.0
import com.nokia.meego 2.0
import "Util.js" as Util

// An event: its title, when and where, how often, the reminder and the
// notes; to change or delete.
Page {
    id: page

    property int eventId
    property date day
    property var ev: store.event(eventId)

    Connections { target: store; onChanged: page.ev = store.event(page.eventId) }

    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop() }
        ToolIcon {
            iconId: "toolbar-edit"
            onClicked: {
                var sheet = sheetComponent.createObject(page)
                sheet.startEdit(page.ev)
                sheet.open()
            }
        }
        ToolIcon { iconId: "toolbar-delete"; onClicked: confirm.open() }
    }

    QueryDialog {
        id: confirm
        titleText: qsTr("Delete event?")
        message: page.ev.recur !== "none" ? qsTr("All its repetitions go too.") : ""
        acceptButtonText: qsTr("Delete")
        rejectButtonText: qsTr("Cancel")
        onAccepted: {
            store.remove(page.eventId)
            pageStack.pop()
        }
    }

    Rectangle { anchors.fill: parent; color: "#f2f2f2" }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: column.height + 32
        Column {
            id: column
            x: 16
            y: 16
            width: parent.width - 32
            spacing: 16

            Rectangle {
                width: parent.width
                height: titleLabel.height + 24
                color: Util.colors[(page.ev.color || 0) % Util.colors.length]
                radius: 4
                Label {
                    id: titleLabel
                    anchors { left: parent.left; right: parent.right; margins: 16; verticalCenter: parent.verticalCenter }
                    text: page.ev.title || qsTr("(No title)")
                    font.pixelSize: 32
                    font.bold: true
                    color: "white"
                    wrapMode: Text.Wrap
                }
            }
            Detail {
                caption: Util.longDate(page.ev.start)
                text: Util.span(page.ev, null)
            }
            Detail {
                visible: page.ev.location !== ""
                caption: qsTr("Location")
                text: page.ev.location || ""
            }
            Detail {
                caption: qsTr("Repeat")
                text: Util.recurText(page.ev.recur)
            }
            Detail {
                caption: qsTr("Alert")
                text: Util.alertText(page.ev.alert)
            }
            Detail {
                visible: page.ev.notes !== ""
                caption: qsTr("Description")
                text: page.ev.notes || ""
            }
        }
    }
    ScrollDecorator { flickableItem: flick }

    Component {
        id: sheetComponent
        EventSheet {}
    }
}
