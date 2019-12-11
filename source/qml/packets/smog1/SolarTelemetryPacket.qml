
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
        // ========== Bottom
        TelemetryObject {
            group: "Bottom solar panel"
            description: "ID"
            content: String(packet.bottom.status.solarPanelId)
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Antenna status"
            content: (function() {
                if (packet.bottom.status.antennaStatus === "0")
                    return "Not deployed"
                else if (packet.bottom.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Timestamp"
            content: formatDate(packet.bottom.timestamp)
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Temperature"
            content: String(packet.bottom.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Light sensor"
            content: String(packet.bottom.lightSensorReading_mV) + " mV"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Input current"
            content: String(packet.bottom.inputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Input voltage"
            content: String(packet.bottom.inputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Output current"
            content: String(packet.bottom.outputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Output voltage"
            content: String(packet.bottom.outputVoltage_mV) + " mV"
        },
        // ========== Front
        TelemetryObject {
            group: "Front solar panel"
            description: "ID"
            content: String(packet.front.status.solarPanelId)
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Antenna status"
            content: (function() {
                if (packet.front.status.antennaStatus === "0")
                    return "Not deployed"
                else if (packet.front.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Timestamp"
            content: formatDate(packet.front.timestamp)
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Temperature"
            content: String(packet.front.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Light sensor"
            content: String(packet.front.lightSensorReading_mV) + " mV"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Input current"
            content: String(packet.front.inputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Input voltage"
            content: String(packet.front.inputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Output current"
            content: String(packet.front.outputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Output voltage"
            content: String(packet.front.outputVoltage_mV) + " mV"
        },
        // ========== Right
        TelemetryObject {
            group: "Right solar panel"
            description: "ID"
            content: String(packet.right.status.solarPanelId)
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Antenna status"
            content: (function() {
                if (packet.right.status.antennaStatus === "0")
                    return "Not deployed"
                else if (packet.right.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Timestamp"
            content: formatDate(packet.right.timestamp)
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Temperature"
            content: String(packet.right.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Light sensor"
            content: String(packet.right.lightSensorReading_mV) + " mV"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Input current"
            content: String(packet.right.inputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Input voltage"
            content: String(packet.right.inputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Output current"
            content: String(packet.right.outputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Output voltage"
            content: String(packet.right.outputVoltage_mV) + " mV"
        },
        // ========== Left
        TelemetryObject {
            group: "Left solar panel"
            description: "ID"
            content: String(packet.left.status.solarPanelId)
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Antenna status"
            content: (function() {
                if (packet.left.status.antennaStatus === "0")
                    return "Not deployed"
                else if (packet.left.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Timestamp"
            content: formatDate(packet.left.timestamp)
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Temperature"
            content: String(packet.left.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Light sensor"
            content: String(packet.left.lightSensorReading_mV) + " mV"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Input current"
            content: String(packet.left.inputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Input voltage"
            content: String(packet.left.inputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Output current"
            content: String(packet.left.outputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Output voltage"
            content: String(packet.left.outputVoltage_mV) + " mV"
        },
        // ========== Back
        TelemetryObject {
            group: "Back solar panel"
            description: "ID"
            content: String(packet.back.status.solarPanelId)
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Antenna status"
            content: (function() {
                if (packet.back.status.antennaStatus === "0")
                    return "Not deployed"
                else if (packet.back.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Timestamp"
            content: formatDate(packet.back.timestamp)
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Temperature"
            content: String(packet.back.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Light sensor"
            content: String(packet.back.lightSensorReading_mV) + " mV"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Input current"
            content: String(packet.back.inputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Input voltage"
            content: String(packet.back.inputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Output current"
            content: String(packet.back.outputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Output voltage"
            content: String(packet.back.outputVoltage_mV) + " mV"
        },
        // ========== Top
        TelemetryObject {
            group: "Top solar panel"
            description: "ID"
            content: String(packet.top.status.solarPanelId)
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Antenna status"
            content: (function() {
                if (packet.top.status.antennaStatus === "0")
                    return "Not deployed"
                else if (packet.top.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Timestamp"
            content: formatDate(packet.top.timestamp)
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Temperature"
            content: String(packet.top.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Light sensor"
            content: String(packet.top.lightSensorReading_mV) + " mV"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Input current"
            content: String(packet.top.inputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Input voltage"
            content: String(packet.top.inputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Output current"
            content: String(packet.top.outputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Output voltage"
            content: String(packet.top.outputVoltage_mV) + " mV"
        }
    ]
}
