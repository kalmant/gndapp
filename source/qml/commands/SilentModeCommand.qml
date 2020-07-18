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
        silentDurationBox.focus = false

        packet.startTimestamp = (scheduleTimeRow.when || (Date.now() / 1000))
        packet.silentDuration = silentDurationBox.value
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Silent mode request command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command sends a timed silent mode request."
    }

    GenericOptions {
        id: options
    }

    CommandTimeRow {
        id: scheduleTimeRow
        caption: "Start time"
        timestamp: packet.startTimestamp
        visible: !sendNowSwitch.checked
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Duration [s]"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: silentDurationBox
            anchors.verticalCenter: parent.verticalCenter
            editable: true
            from: 30
            to: 24 * 3600
            value: packet.silentDuration
        }
    }
}
