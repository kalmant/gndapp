
import QtQuick 2.9
import QtQuick.Controls 2.2

Column {
    id: view

    property var packet
    property alias when: options.when
    property alias repeat: options.repeat

    spacing: 10
    width: 640

    function updatePacket() {
        packet = rawAsciiField.text
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Custom command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "Sends a custom, non-encoded text to the satellite."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            id: rawAsciiLabel
            text: "Raw ASCII command"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField {
            id: rawAsciiField
            selectByMouse: true
            width: parent.width - rawAsciiLabel.width - parent.spacing
            maximumLength: 64
            text: packet
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
