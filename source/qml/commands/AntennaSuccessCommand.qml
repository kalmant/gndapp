
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
        text: "Antenna success command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command tells the satellite that the antenna has been opened successfully, and it should not attempt to automatically open it anymore."
    }

    GenericOptions {
        id: options
    }
}
