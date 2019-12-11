import QtQuick 2.9
import QtQuick.Controls 2.2
import s1obc 1.0
import "commands"

Column {
    id: column

    spacing: 10
    width: parent.width
    anchors.margins: 10

    GroupBox {
        title: "Command queue"
        padding: 1
        height: 100
        width: parent.width

        Rectangle {
            anchors.fill: parent

            ListView {
                anchors.fill: parent
                id: commandListView
                flickableDirection: Flickable.VerticalFlick
                boundsBehavior: Flickable.StopAtBounds
                model: gndConnection.commands
                clip: true
                delegate: Text {
                    text: " " + modelData
                    font.family: defaultMonotypeFontFamily
                }

                ScrollBar.vertical: ScrollBar { }
            }
        }
    }

    Item {
        width: parent.width
        height: connectionRow.height

        Row {
            id: connectionRow
            spacing: 10
            anchors.left: parent.left

            Label {
                text: gndConnection.isConnected ? ("Connected to " + ipInput.text + ":" + portInput.text) : "Disconnected"
                color: gndConnection.isConnected ? "green" : "red"
                anchors.verticalCenter: parent.verticalCenter
            }

            Label {
                text: "IP address"
                enabled: !gndConnection.isConnected
                visible: enabled
                anchors.verticalCenter: parent.verticalCenter
            }

            TextField {
                id: ipInput
                selectByMouse: true
                enabled: !gndConnection.isConnected
                text: gndIp
                maximumLength: 15
                visible: enabled
                anchors.verticalCenter: parent.verticalCenter
            }

            Label {
                text: "Port"
                enabled: !gndConnection.isConnected
                visible: enabled
                anchors.verticalCenter: parent.verticalCenter
            }

            TextField {
                id: portInput
                selectByMouse: true
                text: gndPort
                width: ipInput.width / 2
                enabled: !gndConnection.isConnected
                visible: enabled
                validator: IntValidator {
                    bottom: 1
                    top: 999999
                }
                anchors.verticalCenter: parent.verticalCenter
            }

            Button {
                id: connectButton
                enabled: !gndConnection.isConnected
                visible: enabled
                text: "Connect"
                onClicked: {
                    gndConnection.connectToServer(ipInput.text,
                                                  parseInt(portInput.text))
                }
                anchors.verticalCenter: parent.verticalCenter
            }

            Button {
                id: disconnectButton
                enabled: gndConnection.isConnected
                visible: enabled
                text: "Disconnect"
                onClicked: {
                    gndConnection.disconnectFromServer()
                }
                anchors.verticalCenter: parent.verticalCenter
            }

        }

        Row {
            spacing: 10
            anchors.right: parent.right

            Button {
                text: "Create command"
                onClicked: commandTypesMenu.open()

                Menu {
                    id: commandTypesMenu
                    rightMargin: 10
                    bottomMargin: 35

                    property var lastPackets: []

                    function openCommand(packetType) {
                        var dialog = commandDialogComponent.createObject(mainWindow, {x: mainWindow.x + 40, y: mainWindow.y + 120})
                        dialog.openCommand(packetType, lastPackets[packetType])
                    }

                    MenuItem {
                        text: "Ping"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_Ping)
                    }
                    MenuItem {
                        text: "Toggle"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_Toggle)
                    }
                    MenuItem {
                        text: "Reset"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_Reset)
                    }
                    MenuItem {
                        text: "Set communication parameters"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_SetComParameters)
                    }
                    MenuItem {
                        text: "Start spectrum analyzer"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_StartSpectrumAnalyzer)
                    }
                    MenuItem {
                        text: "Open antenna"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_OpenAntenna)
                    }
                    MenuItem {
                        text: "Antenna open success signal"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_AntennaOpenSuccess)
                    }
                    MenuItem {
                        text: "Set beacon"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_SetBeacon)
                    }
                    MenuItem {
                        text: "File download"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_FileDownloadRequest)
                    }
                    MenuItem {
                        text: "Start measurement"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_MeasurementRequest)
                    }
                    MenuItem {
                        text: "Delete file"
                        onTriggered: commandTypesMenu.openCommand(OBC.UplinkPacketType_FileDelete)
                    }
                    MenuItem {
                        text: "Custom"
                        onTriggered: commandTypesMenu.openCommand(0)
                    }
                }

                Component {
                    id: commandDialogComponent

                    CommandDialog {
                        id: commandDialog

                        onAccepted: {
                            console.warn("enqueueing packet:", packet, "when:", when)
                            gndConnection.queueUplink(repeat, when, packet)
                            commandTypesMenu.lastPackets[packetType] = packet
                            destroy()
                        }
                        onRejected: {
                            commandTypesMenu.lastPackets[packetType] = packet
                            destroy()
                        }
                    }
                }
            }

            Button {
                enabled: gndConnection.isConnected
                visible: enabled
                text: "Clear queue"
                onClicked: gndConnection.clearCommandQueue()
            }

            Button {
                enabled: gndConnection.isConnected
                visible: enabled
                text: "Send queue"
                onClicked: gndConnection.startSendingCommands(0, 500, 15000)
            }
        }
    }
}
