
import QtQuick 2.9
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2 as Dialogs1
import s1obc 1.0
import "packets/smog1" as Smog1
import "packets/smogp" as SmogP
import "packets/atl1" as Atl1

Column {
    id: column

    property var currentPacket
    property string currentPacketString
    property string currentPacketSatelliteName

    anchors.fill: parent
    anchors.margins: 10

    Row {
        id: row
        width: parent.width
        height: parent.height
        spacing: 10

        CustomTableView {
            id: tv
            clip: true
            height: parent.height
            width: parent.width
                   - (packetContentsBox.visible ? (packetContentsBox.width + parent.spacing) : 0)
            Keys.onPressed: {
                if (event.key === Qt.Key_Escape) {
                    currentIndex = -1
                    currentPacket = null
                    event.accepted = true
                }
            }

            headerModel: ListModel {
                ListElement {
                    role: "timestamp"
                    title: qsTr("Timestamp")
                    width: 180
                    movable: false
                }
                ListElement {
                    role: "source"
                    title: qsTr("Source")
                    width: 100
                    movable: false
                }
                ListElement {
                    role: "type"
                    title: qsTr("Type")
                    width: 140
                    movable: false
                }
                ListElement {
                    role: "encoding"
                    title: qsTr("Encoding")
                    width: 100
                    movable: false
                }
                ListElement {
                    role: "satellite"
                    title: qsTr("Satellite")
                    width: 80
                    movable: false
                }
                ListElement {
                    role: "rssi"
                    title: qsTr("RSSI")
                    width: 80
                    movable: false
                }
            }

            model: packetTableModel

            // Change coming from the UI
            onCurrentIndexChanged: {
                var currRow = currentIndex
                if (currRow === -1)
                    return

                // Set packetTableModel value to reflect the UI change
                packetTableModel.selectedRow = currRow

                var newpacket = packetTableModel.getPacket(currRow)

                // Check if packets exist and timestamp+type match
                var samePacket = !!newpacket && !!currentPacket
                        && (newpacket.packetType === currentPacket.packetType)
                        && (newpacket.timestamp === currentPacket.timestamp)

                // Check signature match as well
                if (samePacket) {
                    var newsig = new Uint8Array(newpacket.signature)
                    var oldsig = new Uint8Array(currentPacket.signature)
                    samePacket = Array.prototype.every.call(
                                newsig,
                                function (val, idx) { return val === oldsig[idx] })
                }

                // Only update the UI if the packet is not the same
                if (!samePacket) {
                    currentPacket = newpacket
                    currentPacketString = packetTableModel.readableQString(currRow)
                    currentPacketSatelliteName = packetTableModel.getSatelliteName(currRow)
                }

            }

            // When rows have been inserted, packetTableModel.selectedRow changes
            Connections {
                target: packetTableModel
                onSelectedRowChanged: {
                    if (tv.currentIndex === -1)
                        return

                    if (tv.currentIndex !== packetTableModel.selectedRow) {
                        tv.currentIndex = packetTableModel.selectedRow
                    }
                }
            }

            Item {
                anchors.fill: parent
                anchors.margins: 10
                anchors.topMargin: tv.headerHeight + 10

                Image {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.leftMargin: 50
                    fillMode: Image.PreserveAspectFit
                    verticalAlignment: Image.AlignBottom
                    horizontalAlignment: Image.AlignHCenter
                    width: row.width - 100
                    height: parent.height
                    source: "qrc:/images/smog_art_notext_black.png"
                    opacity: 0.07
                    visible: mainWindow.currentSatellite !== 2
                }
            }
        }

        Item {
            id: packetContentsBox
            visible: !!currentPacket
            width: Math.min(420, mainWindow.width - 200)
            height: parent.height

            MouseArea {
                property int dragStartX
                property int dragStartWidth

                preventStealing: true
                width: 10
                x: -10
                height: parent.height
                cursorShape: Qt.SizeHorCursor

                onPressed: {
                    dragStartX = mapToItem(column, mouse.x, 0).x
                    dragStartWidth = packetContentsBox.width
                }
                onReleased: {
                    var w = packetContentsBox.width
                    packetContentsBox.width = Qt.binding(function() { return Math.min(w, mainWindow.width - 200) })
                }
                onPositionChanged: {
                    if (pressed) {
                        var x = mapToItem(column, mouse.x, 0).x
                        var diff = -x + dragStartX
                        packetContentsBox.width = Math.min(Math.max(dragStartWidth + diff, 200), mainWindow.width - 200)
                    }
                }
            }

            Row {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: -5
                anchors.rightMargin: -5
                z: 10
                visible: !!currentPacket
                spacing: 5

                RoundButton {
                    text: packetMenu.visible ? "▲" : "▼"
                    enabled: !packetMenu.visible
                    onClicked: packetMenu.popup()

                    Menu {
                        id: packetMenu
                        topMargin: parent.height + 68
                        rightMargin: 75

                        Action {
                            text: "Copy raw packet to clipboard"
                            onTriggered: {
                                if (tv.currentIndex !== -1) {
                                    packetTableModel.copyToClipboard(tv.currentIndex)
                                }
                            }
                        }
                    }
                }

                RoundButton {
                    text: "×"
                    onClicked: {
                        tv.currentIndex = -1
                        currentPacket = null
                    }
                }
            }

            Component {
                id: solarTelemetryComponent_smog1

                Smog1.SolarTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: solarTelemetryComponent_smogp

                SmogP.SolarTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: solarTelemetryComponent_atl1

                Atl1.SolarTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: pcuTelemetryComponent_smog1

                Smog1.PcuTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: pcuTelemetryComponent_smogp

                SmogP.PcuTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: pcuTelemetryComponent_atl1

                Atl1.PcuTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: onboardTelemetryComponent_smog1

                Smog1.OnboardTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: onboardTelemetryComponent_smogp

                SmogP.OnboardTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: onboardTelemetryComponent_atl1

                Atl1.OnboardTelemetryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: beaconComponent_smog1

                Smog1.BeaconPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: beaconComponent_smogp

                SmogP.BeaconPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: beaconComponent_atl1

                Atl1.BeaconPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: batteryComponent_atl1

                Atl1.BatteryPacket {
                    anchors.fill: parent
                    packet: currentPacket
                }
            }

            Component {
                id: packetTextAreaComponent

                TextArea {
                    anchors.fill: parent
                    textFormat: TextEdit.PlainText
                    font.pixelSize: 12
                    font.family: defaultMonotypeFontFamily
                    readOnly: true
                    selectByKeyboard: true
                    selectByMouse: true
                    enabled: true
                    text: currentPacketString
                    wrapMode: TextEdit.NoWrap
                }
            }

            Loader {
                id: packetViewLoader
                anchors.fill: parent
                active: !!currentPacket
                sourceComponent: {
                    if (!currentPacket)
                        return null

                    switch (Number(currentPacket.packetType)) {
                    case OBC.DownlinkPacketType_Telemetry1:
                        switch (currentPacketSatelliteName){
                        case "SMOG-1":
                            return solarTelemetryComponent_smog1
                        case "SMOG-P":
                            return solarTelemetryComponent_smogp
                        case "ATL-1":
                            return solarTelemetryComponent_atl1
                        default:
                            return null
                        }
                    case OBC.DownlinkPacketType_Telemetry2:
                        switch (currentPacketSatelliteName){
                        case "SMOG-1":
                            return pcuTelemetryComponent_smog1
                        case "SMOG-P":
                            return pcuTelemetryComponent_smogp
                        case "ATL-1":
                            return pcuTelemetryComponent_atl1
                        default:
                            return null
                        }
                    case OBC.DownlinkPacketType_Telemetry3:
                        switch (currentPacketSatelliteName){
                        case "SMOG-1":
                            return onboardTelemetryComponent_smog1
                        case "SMOG-P":
                            return onboardTelemetryComponent_smogp
                        case "ATL-1":
                            return onboardTelemetryComponent_atl1
                        default:
                            return null
                        }
                    case OBC.DownlinkPacketType_Beacon:
                        switch (currentPacketSatelliteName){
                        case "SMOG-1":
                            return beaconComponent_smog1
                        case "SMOG-P":
                            return beaconComponent_smogp
                        case "ATL-1":
                            return beaconComponent_atl1
                        default:
                            return null
                        }
                    case OBC.DownlinkPacketType_Telemetry3_A:
                        switch (currentPacketSatelliteName){
                        case "ATL-1":
                            return batteryComponent_atl1
                        default:
                            return null
                        }
                    default:
                        return packetTextAreaComponent
                    }
                }
            }
        }
    }
}
