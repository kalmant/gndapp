
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
        idleTimeNormalBox.focus = false
        idleTimeSavingBox.focus = false
        idleTimeEmergencyBox.focus = false
        idleIsOffLimitBox.focus = false

        packet.txDataRate = dataRateModel.get(dataRateCombo.currentIndex).value
        packet.txPowerLevel = powerLevelModel.get(powerLevelCombo.currentIndex).value
        packet.idleTimeNormal = idleTimeNormalBox.value
        packet.idleTimeSaving = idleTimeSavingBox.value
        packet.idleTimeEmergency = idleTimeEmergencyBox.value
        packet.idleIsOffLimit = idleIsOffLimitBox.value
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Set communication parameters command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command sets the communication parameters of telemetry packets."
    }

    GenericOptions {
        id: options
    }

    Row {
        spacing: parent.spacing
        width: parent.width

        Label {
            width: leftColumnWidth
            text: "Data rate"
            height: Math.max(dataRateCombo.height, implicitHeight)
            verticalAlignment: Text.AlignVCenter
        }

        ComboBox {
            id: dataRateCombo
            textRole: "text"
            width: 200
            currentIndex: Number(packet.txDataRate)
            model: ListModel {
                id: dataRateModel

                ListElement {
                    text: "500 bps"
                    value: OBC.ComTxDatarate_500bps
                }
                ListElement {
                    text: "1250 bps"
                    value: OBC.ComTxDatarate_1250bps
                }
                ListElement {
                    text: "2500 bps"
                    value: OBC.ComTxDatarate_2500bps
                }
                ListElement {
                    text: "5000 bps"
                    value: OBC.ComTxDatarate_5000bps
                }
                ListElement {
                    text: "12500 bps"
                    value: OBC.ComTxDatarate_12500bps
                }
                ListElement {
                    text: "25000 bps"
                    value: OBC.ComTxDatarate_25000bps
                }
                ListElement {
                    text: "50000 bps"
                    value: OBC.ComTxDatarate_50000bps
                }
                ListElement {
                    text: "125000 bps"
                    value: OBC.ComTxDatarate_125000bps
                }
            }
        }
    }

    Row {
        spacing: 10
        width: parent.width

        Label {
            width: leftColumnWidth
            text: "Power level"
            height: Math.max(powerLevelCombo.height, implicitHeight)
            verticalAlignment: Text.AlignVCenter
        }

        ComboBox {
            id: powerLevelCombo
            textRole: "text"
            width: 200
            currentIndex: Number(packet.txPowerLevel)
            model: ListModel {
                id: powerLevelModel

                // TODO: add "don't change" option
                ListElement {
                    text: "10 mW (0x00)"
                    value: OBC.ComTxPowerLevel_10mW
                }
                ListElement {
                    text: "?? mW (0x01)"
                    value: OBC.ComTxPowerLevel_1
                }
                ListElement {
                    text: "?? mW (0x02)"
                    value: OBC.ComTxPowerLevel_2
                }
                ListElement {
                    text: "?? mW (0x03)"
                    value: OBC.ComTxPowerLevel_3
                }
                ListElement {
                    text: "?? mW (0x04)"
                    value: OBC.ComTxPowerLevel_4
                }
                ListElement {
                    text: "?? mW (0x05)"
                    value: OBC.ComTxPowerLevel_5
                }
                ListElement {
                    text: "?? mW (0x06)"
                    value: OBC.ComTxPowerLevel_6
                }
                ListElement {
                    text: "25 mW (0x07)"
                    value: OBC.ComTxPowerLevel_25mW
                }
                ListElement {
                    text: "?? mW (0x08)"
                    value: OBC.ComTxPowerLevel_8
                }
                ListElement {
                    text: "?? mW (0x09)"
                    value: OBC.ComTxPowerLevel_9
                }
                ListElement {
                    text: "?? mW (0x0A)"
                    value: OBC.ComTxPowerLevel_A
                }
                ListElement {
                    text: "?? mW (0x0B)"
                    value: OBC.ComTxPowerLevel_B
                }
                ListElement {
                    text: "?? mW (0x0C)"
                    value: OBC.ComTxPowerLevel_C
                }
                ListElement {
                    text: "50 mW (0x0D)"
                    value: OBC.ComTxPowerLevel_50mW
                }
                ListElement {
                    text: "?? mW (0x0E)"
                    value: OBC.ComTxPowerLevel_E
                }
                ListElement {
                    text: "100 mW (0x0F)"
                    value: OBC.ComTxPowerLevel_100mW
                }
            }
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            text: "Idle time (normal):"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: idleTimeNormalBox
            anchors.verticalCenter: parent.verticalCenter
            from: 1000
            to: 15000
            stepSize: 200
            value: packet.idleTimeNormal
            editable: true
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            text: "Idle time (saving):"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: idleTimeSavingBox
            anchors.verticalCenter: parent.verticalCenter
            from: 1000
            to: 15000
            stepSize: 200
            value: packet.idleTimeSaving
            editable: true
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            text: "Idle time (emerg.):"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: idleTimeEmergencyBox
            anchors.verticalCenter: parent.verticalCenter
            from: 1000
            to: 15000
            stepSize: 200
            value: packet.idleTimeEmergency
            editable: true
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            text: "Idle OFF limit:"
            width: leftColumnWidth
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: idleIsOffLimitBox
            anchors.verticalCenter: parent.verticalCenter
            from: 500
            to: 16000
            stepSize: 100
            value: packet.idleIsOffLimit
            editable: true
        }
    }
}
