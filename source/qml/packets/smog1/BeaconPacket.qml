
import QtQuick 2.9
import QtQuick.Controls 2.2
import ".."
PacketDetailsTable {
    model: [
        TelemetryObject {
            group: "-"
            description: "Timestamp"
            content: formatDate(packet.timestamp)
        },
        TelemetryObject {
            group: "-"
            description: "Signature"
            content: formatHexBytes(packet.signature)
        },
        TelemetryObject {
            group: "-"
            description: "ACK1"
            content: "ID=" + packet.acknowledgedCommands[0].commandId
            + " (RSSI=" + packet.acknowledgedCommands[0].receivedRssi + " dBm)"
        },
        TelemetryObject {
            group: "-"
            description: "ACK2"
            content: "ID=" + packet.acknowledgedCommands[1].commandId
            + " (RSSI=" + packet.acknowledgedCommands[1].receivedRssi + " dBm)"
        },
        TelemetryObject {
            group: "-"
            description: "ACK3"
            content: "ID=" + packet.acknowledgedCommands[2].commandId
            + " (RSSI=" + packet.acknowledgedCommands[2].receivedRssi + " dBm)"
        },
        // ========== Beacon
        TelemetryObject {
            group: "Beacon"
            description: "Message"
            content: String(packet.message)
        },
        TelemetryObject {
            group: "Beacon"
            description: "Version info"
            content: String(packet.versionInfo)
        },
        // ========== Diagnostic info
        TelemetryObject {
            group: "Diag"
            description: "OBC flash checksum"
            content: String(packet.diag.flashChecksum)
        },
        TelemetryObject {
            group: "Diag"
            description: "Last uplink timestamp"
            content: formatDate(packet.diag.lastUplinkTimestamp)
        },
        TelemetryObject {
            group: "Diag"
            description: "OBC uptime"
            content: String(packet.diag.obcUptimeMin) + " min"
        },
        TelemetryObject {
            group: "Diag"
            description: "COM uptime"
            content: String(packet.diag.comUptimeMin) + " min"
        },
        TelemetryObject {
            group: "Diag"
            description: "TX voltage drop"
            content: String(packet.diag.txVoltageDrop_10mV * 10.0) + " mV"
        },
        TelemetryObject {
            group: "Diag"
            description: "Timed task count"
            content: String(packet.diag.taskCount)
        },
        TelemetryObject {
            group: "Diag"
            description: "Energy mode"
            content: String(packet.diag.status.energyMode)
        },
        TelemetryObject {
            group: "Diag"
            description: "TCXO works"
            content: packet.diag.status.tcxoWorks ? "OK" : "NO"
        },
        TelemetryObject {
            group: "Diag"
            description: "Filesystem"
            content: packet.diag.status.filesystemOk ? "OK" : "NO"
        },
        TelemetryObject {
            group: "Diag"
            description: "Current flash"
            content: packet.diag.status.filesystemUsesFlash2 ? "Flash 2" : "Flash 1"
        }
    ]
}
