
import QtQuick 2.9
import QtQuick.Controls 2.2

Column {
    id: view

    property var packet
    property alias when: options.when
    property alias repeat: options.repeat

    spacing: 10
    width: 640

    Label {
        font.bold: true
        width: parent.width
        text: "Ping command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command is used to verify that the satellite can receive commands from the ground station."
    }

    GenericOptions {
        id: options
    }
}
