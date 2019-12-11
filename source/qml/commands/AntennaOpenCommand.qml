
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
        // Manually steal focus to make controls commit their editable value
        chargeBox.focus = false
        dischargeBox.focus = false

        if (presetBox.currentIndex === 0) {
            packet.batteryPwmTime = dischargeBox.value
            packet.capacitorPwmChargeTime = 0
            packet.capacitorPwmDischargeTime = 0
        } else {
            packet.batteryPwmTime = 0
            packet.capacitorPwmChargeTime = chargeBox.value
            packet.capacitorPwmDischargeTime = dischargeBox.value
        }
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Open antenna command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command tells the satellite to attempt to open its antenna."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "How?"
            anchors.verticalCenter: parent.verticalCenter
        }

        ComboBox {
            id: presetBox

            property bool useCapacitors: currentIndex !== 0

            anchors.verticalCenter: parent.verticalCenter
            currentIndex: packet.batteryPwmTime ? 0 : 1
            textRole: "text"
            model: ListModel {
                ListElement {
                    text: "Battery"
                }
                ListElement {
                    text: "Battery and capacitors"
                }
            }
            width: 200
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing
        visible: presetBox.useCapacitors

        Label {
            width: leftColumnWidth
            text: "Charge time [ms]"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: chargeBox
            anchors.verticalCenter: parent.verticalCenter
            editable: true
            from: 1000
            to: 60 * 60 * 1000
            value: packet.capacitorPwmChargeTime
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Discharge time [ms]"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: dischargeBox
            anchors.verticalCenter: parent.verticalCenter
            editable: true
            from: 1000
            to: 20000
            value: packet.batteryPwmTime || packet.capacitorPwmDischargeTime
        }
    }
}
