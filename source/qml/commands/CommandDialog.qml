import QtQuick 2.9
import QtQuick.Controls 2.2
import s1obc 1.0

ApplicationWindow {
    id: commandDialog

    property var packet
    property var repeat: !!loader.item ? loader.item.repeat : 0
    property var when: !!loader.item ? loader.item.when : 0
    property int packetType: -1
    property int leftColumnWidth: 150

    signal accepted
    signal rejected

    modality: Qt.ApplicationModal
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    minimumWidth: dialogContent.width
    minimumHeight: dialogContent.height
    width: minimumWidth
    height: minimumHeight
    title: "Create command"

    function open() {
        show()
    }

    function openCommand(type, packet) {
        if (!packet) {
            commandDialog.packet = (type === 0) ? "" : Packet.createUplink(type)
        } else {
            commandDialog.packet = packet
        }
        commandDialog.packetType = type
        open()
    }

    Column {
        id: dialogContent
        spacing: 10
        padding: 20

        Loader {
            id: loader

            sourceComponent: {
                // TODO: custom command
                switch (Number(packetType)) {
                case 0:
                    return customComponent
                case OBC.UplinkPacketType_Ping:
                    return pingComponent
                case OBC.UplinkPacketType_Toggle:
                    return toggleComponent
                case OBC.UplinkPacketType_Reset:
                    return resetComponent
                case OBC.UplinkPacketType_SetComParameters:
                    return comSetComponent
                case OBC.UplinkPacketType_StartSpectrumAnalyzer:
                    return spectrumStartComponent
                case OBC.UplinkPacketType_OpenAntenna:
                    return antennaOpenComponent
                case OBC.UplinkPacketType_AntennaOpenSuccess:
                    return antennaSuccessComponent
                case OBC.UplinkPacketType_SetBeacon:
                    return beaconSetComponent
                case OBC.UplinkPacketType_FileDownloadRequest:
                    return fileDownloadComponent
                case OBC.UplinkPacketType_MeasurementRequest:
                    return measurementRequestComponent
                case OBC.UplinkPacketType_FileDelete:
                    return fileDeleteComponent
                default:
                    return null
                }
            }

            Component {
                id: pingComponent

                PingCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: toggleComponent

                ToggleCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: resetComponent

                ResetCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: comSetComponent

                ComSetCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: spectrumStartComponent

                SpectrumStartCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: antennaOpenComponent

                AntennaOpenCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: antennaSuccessComponent

                AntennaSuccessCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: beaconSetComponent

                BeaconSetCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: fileDownloadComponent

                FileDownloadCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: measurementRequestComponent

                MeasurementRequestCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: fileDeleteComponent

                FileDeleteCommand {
                    packet: commandDialog.packet
                }
            }

            Component {
                id: customComponent

                CustomCommand {
                    packet: commandDialog.packet
                }
            }
        }

        DialogButtonBox {
            anchors.right: parent.right
            anchors.rightMargin: 15
            standardButtons: Dialog.Ok | Dialog.Cancel
            onAccepted: {
                if (loader.item && loader.item.updatePacket) {
                    loader.item.updatePacket()
                    if (loader.item.packet !== packet) {
                        packet = loader.item.packet
                    }
                }

                commandDialog.accepted()
            }
            onRejected: {
                commandDialog.rejected()
            }
        }
    }

}

