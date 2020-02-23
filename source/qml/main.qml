
import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2 as Dialogs1
import s1obc 1.0

ApplicationWindow {
    id: mainWindow

    title: qsTr("SMOG-1 GND Client Software")
    visible: true
    minimumWidth: 800
    minimumHeight: 600
    height: Screen.desktopAvailableHeight <= 900 ? 600 : 900
    width: Screen.desktopAvailableWidth <= 1366 ? 800 : 1200

    property int margin: 10
    property bool showBottomSection: true
    property int currentSatellite: 0

    property var lastTelemetry1packet_smog1: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry1, 0)
    property var lastTelemetry2packet_smog1: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry2, 0)
    property var lastTelemetry3packet_smog1: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry3, 0)
    property var lastTelemetry4packet_smog1: Packet.createDownlink(OBC.DownlinkPacketType_Beacon, 0)
    property var lastTelemetry1packet_smogp: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry1, 1)
    property var lastTelemetry2packet_smogp: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry2, 1)
    property var lastTelemetry3packet_smogp: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry3, 1)
    property var lastTelemetry4packet_smogp: Packet.createDownlink(OBC.DownlinkPacketType_Beacon, 1)
    property var lastTelemetry1packet_atl1: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry1, 2)
    property var lastTelemetry2packet_atl1: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry2, 2)
    property var lastTelemetry3packet_atl1: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry3, 2)
    property var lastTelemetry4packet_atl1: Packet.createDownlink(OBC.DownlinkPacketType_Beacon, 2)
    property var lastTelemetry5packet_atl1: Packet.createDownlink(OBC.DownlinkPacketType_Telemetry3_A, 2)

    onClosing: {
        if (uploader.isCurrentlyLoggingIn || uploader.isCurrentlyUploading){
            uploader.logout()
        }
        switch(settingsTab.item.saveSettingsOnExit){
        case 0:
            settingsTab.item.saveSettings();
            Qt.quit();
            break;
        case 1:
            Qt.quit();
            break;
        case 2:
            close.accepted = false;
            exitDialog.open();
            break;
        default:
            exitDialog.open();
            break;
        }
    }

    footer: Item {
        width: parent.width
        height: footerLeftRow.height + 20

        Rectangle {
            height: 1
            width: parent.width
            color: palette.mid
        }

        Row {
            id: footerLeftRow
            spacing: 10
            y: 10
            x: 10

            Label {
                id: uploaderLabel
                text: qsTr("Pending packets: ") + uploader.remaining
            }

            Rectangle {
                width: 1
                height: parent.height
                color: palette.mid
            }

            Label {
                text: qsTr("Recent upload errors: ") + uploader.errorsInARow
            }

            Rectangle {
                width: 1
                height: parent.height
                color: palette.mid
            }

            Label {
                id: datarateLabel
                property int datarate: 5000
                text: qsTr("Datarate: " + datarate)
            }

            Rectangle {
                width: 1
                height: parent.height
                color: palette.mid
            }

            Label {
                id: packetLengthLabel
                property int packetLength: 333
                text: qsTr("Packet length: " + packetLength)
            }
        }

        Row {
            id: footerRightRow
            spacing: 10
            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
                margins: 10
            }

            Label {
                id: ppsLabel
                text: qsTr("Packets received: ") + packetTableModel.currentPPS
            }
        }
    }

    Connections {
        target: packetTableModel
        onPacketAdded: {
            if (!packet)
                return

            switch (Number(packet.packetType)) {
            case OBC.DownlinkPacketType_Telemetry1:
                switch (currentSatellite){
                case 0:
                    lastTelemetry1packet_smog1 = packet
                    break
                case 1:
                    lastTelemetry1packet_smogp = packet
                    break
                case 2:
                    lastTelemetry1packet_atl1 = packet
                    break
                default:
                    break
                }
                break
            case OBC.DownlinkPacketType_Telemetry2:
                switch (currentSatellite){
                case 0:
                    lastTelemetry2packet_smog1 = packet
                    break
                case 1:
                    lastTelemetry2packet_smogp = packet
                    break
                case 2:
                    lastTelemetry2packet_atl1 = packet
                    break
                default:
                    break
                }
                break
            case OBC.DownlinkPacketType_Telemetry3:
                switch (currentSatellite){
                case 0:
                    lastTelemetry3packet_smog1 = packet
                    break
                case 1:
                    lastTelemetry3packet_smogp = packet
                    break
                case 2:
                    lastTelemetry3packet_atl1 = packet
                    break
                default:
                    break
                }
                break
            case OBC.DownlinkPacketType_Beacon:
                switch (currentSatellite){
                case 0:
                    lastTelemetry4packet_smog1 = packet
                    break
                case 1:
                    lastTelemetry4packet_smogp = packet
                    break
                case 2:
                    lastTelemetry4packet_atl1 = packet
                    break
                default:
                    break
                }
                break
            case OBC.DownlinkPacketType_Telemetry3_A:
                switch (currentSatellite){
                case 2:
                    lastTelemetry5packet_atl1 = packet
                    break
                default:
                    break
                }
                break
            }
        }
    }

    Column {
        id: mainLayout
        anchors {
            fill: parent
            margins: mainWindow.margin
        }

        property int tabHeight: mainWindow.height
                                - mainLayout.anchors.topMargin
                                - mainLayout.anchors.bottomMargin
                                - topRow.height
                                - mainWindow.footer.height
                                - (cp.visible ? (cp.height + 10) : 0)
                                - (spectogramComponent.visible ? (spectogramComponent.height + 10) : 0)

        Row {
            id: topRow
            spacing: 10
            width: parent.width

            Button {
                id: optionsButton
                text: optionsMenu.visible ? "Options ▲" :  "Options ▼"
                flat: true
                anchors.bottom: parent.bottom
                enabled: !optionsMenu.visible
                onClicked: optionsMenu.open()

                Menu {
                    id: optionsMenu
                    width: 300
                    topMargin: parent.height + 9

                    Component.onCompleted: {
                        if (!uplinkEnabled) { // Hide actions that should not appear if uplink is not enabled
                            optionsMenu.removeAction(hideCommandMenuMenuItem)
                            optionsMenu.removeAction(commandMenuItem)
                            optionsMenu.removeAction(commandTrackerMenuItem)
                        }
                    }

                    Action {
                        text: "Save settings" + " (" + shortcut + ")"
                        shortcut: "Ctrl+S"
                        onTriggered: settingsTab.item.saveSettings();
                    }

                    Action {
                        text: "Load packets" + " (" + shortcut + ")"
                        shortcut: "Ctrl+O"
                        onTriggered: manualPacketInputLoader.open();
                    }

                    Action {
                        id: hideCommandMenuMenuItem
                        property var sp: null
                        text: "Hide bottom section" + " (" + shortcut + ")"
                        enabled: uplinkEnabled && cp.visible
                        shortcut: "Ctrl+H"
                        onTriggered: {
                            if (sp === null) {
                                console.warn("SettingsPage didn't properly set hideCommandMenuMenuItem's property!")
                            } else {
                                showBottomSection = false
                            }
                        }
                    }                    

                    Action {
                        id: commandMenuItem
                        property var sp: null
                        text: "Show commands" + " (" + shortcut + ")"
                        enabled: uplinkEnabled && !cp.visible

                        shortcut: "Ctrl+M"
                        onTriggered: {
                            if (sp === null){
                                console.warn("SettingsPage didn't properly set commandMenuItem's property!")
                            } else {
                                showBottomSection = true
                            }
                        }
                    }

                    Action {
                        id: commandTrackerMenuItem
                        text: "Open command tracker" + " (" + shortcut + ")"
                        shortcut: "Ctrl+T"
                        enabled: uplinkEnabled
                        onTriggered: {
                            commandTrackerDialog.open()
                        }
                    }

                    Action {
                        id: aboutMenuItem
                        text: "About ..."
                        onTriggered: {
                            aboutDialog.open()
                        }
                    }
                }
            }

            TabBar {
                id: tabBar
                anchors.bottom: parent.bottom
                currentIndex: 0
                width: parent.width
                       - parent.spacing
                       - optionsButton.width

                TabButton {
                    text: qsTr("Settings")
                    width: implicitWidth + 20
                }

                TabButton {
                    text: qsTr("Packets")
                    width: implicitWidth + 20
                }

                TabButton {
                    text: qsTr("Status")
                    width: implicitWidth + 20
                }

            }
        }

        Column {
            spacing: 10
            width: parent.width
            height: parent.height

            Frame {
                height: mainLayout.tabHeight
                width: parent.width
                background: Rectangle {
                    width: parent.width
                    height: parent.height + 1
                    y: -1
                    color: palette.base
                    border.color: palette.mid
                }

                Loader {
                    id: settingsTab
                    visible: tabBar.currentIndex === 0
                    anchors.fill: parent
                    active: true // TODO: find a way to unload the settings page when not visible
                    sourceComponent: SettingsPage {
                        id: settingsPage

                        Component.onCompleted: {
                            commandMenuItem.sp = settingsPage
                        }
                    }
                }

                Loader {
                    visible: tabBar.currentIndex === 1
                    anchors.fill: parent
                    active: visible
                    onActiveChanged: if (active) active = true // Don't unload once loaded
                    sourceComponent: PacketPage {}
                }

                Loader {
                    visible: tabBar.currentIndex === 2
                    anchors.fill: parent
                    active: visible
                    sourceComponent: StatusPage {}
                }

            }

            Loader {
                id: cp
                width: parent.width
                visible: uplinkEnabled && showBottomSection
                active: visible

                sourceComponent: CommandPage {
                    width: parent.width
                }
            }

            SpectogramComponent {
                // TODO: wrap in a Loader so it is only loaded when visible
                id: spectogramComponent
                visible: false
                width: parent.width
            }
        }
    }
    Connections{
        target: gndConnection
        onNewDatarate:{
            datarateLabel.datarate = datarateBPS
        }
        onNewPacketLength:{
            packetLengthLabel.packetLength = packetLength
        }
    }

    Dialogs1.MessageDialog{
        id: exitDialog
        title: qsTr("Save changes")
        text: qsTr("Do you want to save your settings?")
        icon: Dialogs1.StandardIcon.Warning
        standardButtons: Dialogs1.StandardButton.Yes | Dialogs1.StandardButton.No | Dialogs1.StandardButton.Cancel
        onYes: {
            // Save then exit
            settingsTab.item.saveSettings();
            Qt.quit();
        }
        onNo: {
            // Exit without saving
            Qt.quit();
        }
    }
    Dialogs1.MessageDialog{
        id: aboutDialog

        title: qsTr("About GND Client Software")
        icon: Dialogs1.StandardIcon.Information
        standardButtons: Dialogs1.StandardButton.Ok
        text: qsTr("This program was created for the purpose of tracking and collecting telemetry from SMOG-1, SMOG-P and ATL-1."+
                   "\n\nIf you encounter any issues, please get in contact with us at bmegnd@gnd.bme.hu!"+
                   "\n\nStay up to date through https://gnd.bme.hu/smog"+
                   "\n\nThank you to our contributors:\n• OM3BC"+
                   "\n\nVersion: 1.1.1")
    }
    Loader {
        id: manualPacketInputLoader
        active: false
        anchors.centerIn: parent
        onLoaded: {
            item.open()
        }

        function open() {
            if (item) {
                item.open()
            } else {
                active = true
            }
        }

        sourceComponent: Dialog {
            id: manualPacketInput
            title: qsTr("Manual packet input")
            standardButtons: Dialog.Open | Dialog.Cancel
            width: 600
            height: 350
            modal: true
            anchors.centerIn: parent
            onAccepted: {
                var packets = packetInput.text.split("\n")
                for (var i = 0; i < packets.length; i++){
                    var trimmed = packets[i].trim()
                    var words = trimmed.split(" ")
                    var lastWord = words[words.length - 1]
                    if (trimmed.length>0){
                        packetDecoder.manualPacketInput("MANUAL", lastWord)
                    }
                }
                packetInput.text = ''
            }
            onRejected: {
                packetInput.text = ''
            }
            onVisibleChanged: {
                if (visible) {
                    packetInput.forceActiveFocus()
                }
            }

            Column {
                spacing: 10

                Label {
                    id: manualPacketInputInfoLabel
                    text: qsTr("Please provide the packet(s) to be decoded in uppercase hexadecimal string format")
                }

                Frame {
                    id: manualPacketInputFrame
                    padding: 0 // TextArea already has enough padding
                    width: manualPacketInput.width - 2 * manualPacketInput.padding
                    height: manualPacketInput.height
                            - 2 * manualPacketInput.padding
                            - manualPacketInput.header.height
                            - manualPacketInput.footer.height
                            - manualPacketInputInfoLabel.height
                            - parent.spacing

                    ScrollView {
                        anchors.fill: parent
                        clip: true

                        TextArea {
                            id: packetInput
                            width: parent.width - 2
                            height: parent.height - 2
                            font: defaultMonotypeFontFamily
                            focus: true
                            selectByMouse: true
                            selectByKeyboard: true
                            wrapMode: TextEdit.NoWrap
                        }
                    }

                }
            }
        }
    }
    Component {
        id: commandTrackerDelegate
        Item {
            id: containerItem
            width: 400; height: commandColumn.height + 5
            Column {
                anchors.margins: 5
                anchors.centerIn: parent
                id: commandColumn
                Text { text: "<b>Uplink ID:</b> " + uplinkId }
                Text { text: "<b>Command:</b> "}
                Text { text: commandString; font: defaultMonotypeFontFamily; wrapMode: Text.Wrap; width: containerItem.width - 10 }
                Text { text: "<b>Sent:</b> " + sentTimeString }
                Text { text: acknowledged ? ("<b>Acknowledged</b>: " + acknowledgedTimeString) : "<b>Not yet acknowledged</b>"; color: acknowledged ? "green" : "red" }
                Rectangle {color: "steelblue"; width: parent.width; height: 2}
            }
        }
    }
    Dialogs1.Dialog{
        id: commandTrackerDialog
        modality: Qt.NonModal
        title: qsTr("Command tracker")
        standardButtons: Dialogs1.StandardButton.Close

        width: 450
        height: 600

        Column {
            Label{text: qsTr("Commands sent recently:")}
            Rectangle{
                width: commandTrackerDialog.width-25
                height: commandTrackerDialog.height-70
                color: "white"

                ListView{
                    clip: true
                    anchors.fill: parent
                    anchors.margins: 5
                    model: commandTracker

                    delegate: commandTrackerDelegate
                }
            }
        }
    }
}
