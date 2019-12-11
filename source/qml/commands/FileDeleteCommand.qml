
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
        packet.fileName = fileNameField.text
        packet.fileNameIsFilter = filterSwitch.checked
    }

    Label {
        font.bold: true
        width: parent.width
        text: "File delete command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command tells the satellite to delete the matching file, or multiple files."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width - x
        spacing: parent.spacing

        Label {
            id: fileNameLabel
            width: leftColumnWidth
            text: "File name:"
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField {
            id: fileNameField
            selectByMouse: true
            width: Math.floor((parent.width - fileNameLabel.width - parent.spacing) * 0.66)
            maximumLength: 10
            text: packet.fileName
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Filter"
            anchors.verticalCenter: parent.verticalCenter
        }

        Switch {
            id: filterSwitch
            checked: packet.fileNameIsFilter
        }

        Label {
            text: "Delete the first 10 matching files"
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
