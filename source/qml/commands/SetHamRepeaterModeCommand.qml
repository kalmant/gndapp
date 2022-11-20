import QtQuick 2.9
import QtQuick.Controls 2.2
import s1obc 1.0

Column {
    id: view

    property var packet
    property alias when: options.when
    property alias repeat: options.repeat

    spacing: 10
    width: 640

    function updatePacket() {
        packet.aesKey = aesKeyField.text
        packet.enable = enableSwitch.checked
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Set HAM repeater mode"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command tells the satellite to engage the HAM repeater mode."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width - x
        spacing: parent.spacing

        Label {
            id: aesKeyLabel
            width: leftColumnWidth
            text: "AES key:"
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField {
            id: aesKeyField
            selectByMouse: true
            width: Math.floor((parent.width - aesKeyLabel.width - parent.spacing) * 0.66)
            maximumLength: 16
            text: packet.aesKey
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Enable"
            anchors.verticalCenter: parent.verticalCenter
        }

        Switch {
            id: enableSwitch
            checked: packet.enable > 0
        }

        Label {
            text: "If set, HAM repeater mode will be engaged"
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
