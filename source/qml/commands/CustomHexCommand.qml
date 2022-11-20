import QtQuick 2.9
import QtQuick.Controls 2.2

Column {
    id: view

    property var packet
    property alias when: options.when
    property alias repeat: options.repeat

    spacing: 10
    width: 640

    function hexcharcodetonum(c) {
        if (c >= 48 && c <= 57)
            return c - 48
        if (c >= 65 && c <= 70)
            return c - 65 + 10
        return 0
    }

    function numtohexcharcode(x) {
        if (x >= 0 && x <= 9)
            return 48 + x
        if (x >= 10 && x <= 15)
            return 65 + x - 10
        return 48
    }

    function updatePacket() {
        var txt = rawHexField.text.toUpperCase()
        var buffer = new ArrayBuffer(64)
        var view = new Uint8Array(buffer)


        for (var i = 0; i < txt.length; i += 2) {
            var c1 = txt.charCodeAt(i) || 0
            var c2 = txt.charCodeAt(i + 1) || 0

            var x1 = hexcharcodetonum(c1)
            var x2 = hexcharcodetonum(c2)

            var x = x1 * 16 + x2
            view[i / 2] = x
        }

        packet = buffer
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Custom HEX command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "Sends a custom, non-encoded HEX byte array to the satellite."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            id: rawHexLabel
            text: "Raw HEX command"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField {
            id: rawHexField
            selectByMouse: true
            width: parent.width - rawHexLabel.width - parent.spacing
            maximumLength: 128
            text: {
                var view = new Uint8Array(packet)
                var str = ""

                for (var i = 0; i < packet.byteLength; i += 1) {
                    var x = view[i]
                    var x1 = Math.floor(x / 16)
                    var x2 = Math.floor(x - (x1 * 16))

                    var c1 = String.fromCharCode(numtohexcharcode(x1))
                    var c2 = String.fromCharCode(numtohexcharcode(x2))

                    str += c1
                    str += c2
                }

                return str
            }
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
