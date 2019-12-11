
import QtQuick 2.9
import QtQuick.Controls 2.2

Column {
    property alias when: timeRow.when
    property alias repeat: repeatBox.value

    width: parent.width
    spacing: 10

    CommandTimeRow {
        id: timeRow
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Repeat count"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: repeatBox
            anchors.verticalCenter: parent.verticalCenter
            from: 1
            to: 10
            value: 1
            editable: true
        }
    }
}
