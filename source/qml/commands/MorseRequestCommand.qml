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
        packet.startTimestamp = sendNowSwitch.checked ? 0 : (scheduleTimeRow.when || (Date.now() / 1000))
        packet.morseMessage = messageBox.text
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Morse request command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command sends a timed morse request."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Send now"
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }

        Row {
            width: parent.width - leftColumnWidth - parent.spacing
            spacing: parent.spacing

            Switch {
                id: sendNowSwitch
                checked: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    CommandTimeRow {
        id: scheduleTimeRow
        caption: "Morse time"
        timestamp: packet.startTimestamp
        visible: !sendNowSwitch.checked
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            text: "Message"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField {
            id: messageBox
            maximumLength: 12
            text: packet.morseMessage
            selectByMouse: true
        }
    }
}
