import QtQuick 2.0
import com.nokia.meego 2.0
import "Calc.js" as Calc

// The calculation line above (the number or the calculation so far, in
// the calculator's orange, smaller as it grows; backspace at its side;
// a long press copies or pastes), the keys below: in portrait the
// functions above the numbers, in landscape beside them. The keyboard
// works too (digits, + - * /, Enter, Backspace, Escape).
Page {
    id: page

    readonly property bool landscape: width > height

    // the calculation: the first number and the operator, the number
    // being written; after = the result stands until the next key
    property var acc: null
    property string op: ""
    property string entry: ""
    property bool fresh: false

    readonly property string point: Qt.locale().decimalPoint
    function show(s) { return s.replace("-", "−").replace(".", point) }
    readonly property string line: {
        if (acc === null)
            return show(entry !== "" ? entry : "0")
        return show(Calc.format(acc)) + Calc.symbols[op] + show(entry)
    }

    function warn(text) {
        banner.text = text
        banner.show()
    }
    function digit(d) {
        if (fresh) { entry = ""; fresh = false }
        if (Calc.digits(entry) >= Calc.MAX_DIGITS)
            return warn(qsTr("Maximum number of digits reached"))
        entry = entry === "0" ? d : entry === "-0" ? "-" + d : entry + d
    }
    function decimal() {
        if (fresh) { entry = ""; fresh = false }
        if (entry.indexOf(".") < 0)
            entry = (entry === "" || entry === "-" ? entry + "0" : entry) + "."
    }
    function sign() {
        if (entry === "" && acc !== null)
            return
        if (fresh) fresh = false
        var e = entry === "" ? "0" : entry
        entry = e.charAt(0) === "-" ? e.substring(1) : "-" + e
    }
    function operator(o) {
        fresh = false
        if (entry === "" || entry === "-") {
            if (acc === null) acc = 0
            op = o
            entry = ""
            return
        }
        var b = parseFloat(entry)
        if (acc !== null && op !== "") {
            var r = Calc.compute(acc, op, b)
            if (r === null)
                return warn(qsTr("Not defined"))
            acc = r
        } else {
            acc = b
        }
        op = o
        entry = ""
    }
    function equals() {
        if (acc === null || op === "" || entry === "" || entry === "-")
            return
        var r = Calc.compute(acc, op, parseFloat(entry))
        if (r === null)
            return warn(qsTr("Not defined"))
        acc = null
        op = ""
        entry = Calc.format(r)
        fresh = true
    }
    function clear() {
        acc = null; op = ""; entry = ""; fresh = false
    }
    function backspace() {
        if (fresh)
            return clear()
        if (entry !== "") {
            entry = entry.substring(0, entry.length - 1)
            if (entry === "-") entry = ""
        } else if (acc !== null) {
            entry = Calc.format(acc)
            acc = null
            op = ""
        }
    }
    function paste(text) {
        var t = text.trim().replace(point, ".").replace("−", "-").replace(/\s/g, "")
        if (!/^-?[0-9]*\.?[0-9]+(e[-+]?[0-9]+)?$/i.test(t))
            return warn(qsTr("Not a number"))
        if (fresh) fresh = false
        entry = Calc.format(parseFloat(t))
    }

    focus: true
    onStatusChanged: if (status === PageStatus.Active) forceActiveFocus()
    Keys.onPressed: {
        var t = event.text
        if (t >= "0" && t <= "9" && t.length === 1) digit(t)
        else if (t === "." || t === ",") decimal()
        else if (t === "+" || t === "-" || t === "*" || t === "/") operator(t)
        else if (t === "=" || event.key === Qt.Key_Return || event.key === Qt.Key_Enter) equals()
        else if (event.key === Qt.Key_Backspace) backspace()
        else if (event.key === Qt.Key_Escape || event.key === Qt.Key_Delete) clear()
        else return
        event.accepted = true
    }

    Rectangle { anchors.fill: parent; color: "black" }

    // the calculation line
    Item {
        id: lineBox
        anchors { left: parent.left; right: parent.right; top: parent.top
                  leftMargin: page.landscape ? 45 : 32; rightMargin: 8; topMargin: page.landscape ? 0 : 35 }
        height: page.landscape ? 100 : 146
        Text {
            id: lineText
            anchors { left: parent.left; right: back.left; rightMargin: 8; verticalCenter: parent.verticalCenter }
            height: parent.height
            text: page.line
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.family: UiConstants.DefaultFontFamilyLight
            font.pixelSize: page.landscape ? 80 : 120
            fontSizeMode: Text.HorizontalFit
            minimumPixelSize: 26
            color: "#ed9507"
        }
        MouseArea {
            anchors.fill: lineText
            onPressAndHold: menu.open()
        }
        Item {
            id: back
            anchors { right: parent.right; verticalCenter: parent.verticalCenter }
            width: 76; height: 76
            Image {
                anchors.centerIn: parent
                source: "image://theme/icon-m-telephony-backspace" + (backArea.pressed ? "-selected" : "")
            }
            MouseArea {
                id: backArea
                anchors.fill: parent
                onClicked: page.backspace()
                onPressAndHold: page.clear()
            }
        }
    }

    // the keys: the functions and the numbers, each a grid filling its part
    Item {
        id: keys
        anchors { left: parent.left; right: parent.right; top: lineBox.bottom; bottom: parent.bottom
                  leftMargin: page.landscape ? 45 : 24; rightMargin: page.landscape ? 64 : 24
                  topMargin: page.landscape ? 0 : 14; bottomMargin: 14 }

        Grid {
            id: functions
            columns: page.landscape ? 2 : 3
            x: page.landscape ? numbers.width + 40 : 0
            width: page.landscape ? keys.width - numbers.width - 40 : keys.width
            readonly property real cellWidth: width / columns
            readonly property real cellHeight: page.landscape ? keys.height / 3 : keys.height * 9 / 62
            Repeater {
                model: [ { t: "C", f: "clear" }, { t: "±", f: "sign" }, { t: "÷", f: "/" },
                         { t: "×", f: "*" }, { t: "−", f: "-" }, { t: "+", f: "+" } ]
                CalcButton {
                    width: functions.cellWidth
                    height: functions.cellHeight
                    function_: true
                    text: modelData.t
                    onClicked: {
                        if (modelData.f === "clear") page.clear()
                        else if (modelData.f === "sign") page.sign()
                        else page.operator(modelData.f)
                    }
                }
            }
        }

        Grid {
            id: numbers
            columns: 3
            y: page.landscape ? 0 : functions.height
            width: page.landscape ? Math.min(keys.width * 0.55, 3 * keys.height / 4 * 1.2) : keys.width
            height: keys.height - y
            readonly property real cellWidth: width / 3
            readonly property real cellHeight: height / 4
            Repeater {
                model: ["7", "8", "9", "4", "5", "6", "1", "2", "3", "0", ".", "="]
                CalcButton {
                    width: numbers.cellWidth
                    height: numbers.cellHeight
                    text: modelData === "." ? page.point : modelData
                    color: modelData === "=" ? "#ed9507" : modelData === "." ? "#c8c8c8" : "white"
                    onClicked: {
                        if (modelData === "=") page.equals()
                        else if (modelData === ".") page.decimal()
                        else page.digit(modelData)
                    }
                }
            }
        }
    }

    // copy and paste through a hidden text field (QML has no clipboard)
    TextInput { id: clip; visible: false }
    ContextMenu {
        id: menu
        onStatusChanged: if (status === DialogStatus.Closed) page.forceActiveFocus()
        MenuLayout {
            MenuItem {
                text: qsTr("Copy")
                onClicked: {
                    clip.text = page.acc === null ? page.show(page.entry !== "" ? page.entry : "0").replace("−", "-") : page.line
                    clip.selectAll()
                    clip.copy()
                }
            }
            MenuItem {
                text: qsTr("Paste")
                onClicked: {
                    clip.text = ""
                    clip.paste()
                    page.paste(clip.text)
                }
            }
        }
    }
}
