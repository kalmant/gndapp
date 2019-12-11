
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
        // ========== Diagnostic info
        TelemetryObject {
            group: "Diag"
            description: "Valid packets"
            content: String(packet.diag.validPackets)
        },
        TelemetryObject {
            group: "Diag"
            description: "Wrong sized packets"
            content: String(packet.diag.wrongSizedPackets)
        },
        TelemetryObject {
            group: "Diag"
            description: "Packets w/ failed Golay"
            content: String(packet.diag.packetsWithFailedGolayDecoding)
        },
        TelemetryObject {
            group: "Diag"
            description: "Packets w/ wrong signature"
            content: String(packet.diag.packetsWithWrongSignature)
        },
        TelemetryObject {
            group: "Diag"
            description: "Packets w/ invalid serial"
            content: String(packet.diag.packetsWithInvalidSerialNumber)
        },
        TelemetryObject {
            group: "Diag"
            description: "OBC-COM UART errors"
            content: String(packet.diag.uartErrorCounter)
        }
    ]
}
