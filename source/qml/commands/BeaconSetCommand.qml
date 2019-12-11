
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
        positionBox.focus = false

        if (clearSwitch.checked) {
            packet.startIndex = 80
            packet.beaconFragment = ""
        } else {
            packet.startIndex = positionBox.value
            packet.beaconFragment = messageBox.text
        }
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Beacon set command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command sets a fragment the beacon message of the satellite. You need multiple commands to set a full beacon message."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            text: "Clear beacon?"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        Switch {
            id: clearSwitch
            checked: packet.startIndex === 80 && packet.beaconFragment === ""
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing
        visible: !clearSwitch.checked

        Label {
            text: "Message"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField {
            id: messageBox
            maximumLength: 20
            text: packet.beaconFragment
            selectByMouse: true
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing
        visible: !clearSwitch.checked

        Label {
            text: "Position"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: positionBox
            editable: true
            from: 0
            to: Math.min(clearSwitch.checked ? 80 : 79, 80 - messageBox.text.length)
            value: packet.startIndex
        }
    }
}
