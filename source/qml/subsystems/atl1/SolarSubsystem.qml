
import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../packets"

Subsystem {
    shortName: "Solar"
    title: "Latest data about the solar panels"
    model: [
        // ========== Bottom
        TelemetryObject {
            group: "Bottom solar panel"
            description: "ID"
            value: telemetry1packet.bottom.status.solarPanelId
            min: 1
            max: 6
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Antenna status"
            content: (function() {
                if (telemetry1packet.bottom.status.antennaStatus === "0")
                    return "Not deployed"
                else if (telemetry1packet.bottom.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Timestamp"
            value: telemetry1packet.bottom.timestamp
            min: 1
            content: formatDate(telemetry1packet.bottom.timestamp)
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Temperature"
            value: telemetry1packet.bottom.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Light sensor"
            value: telemetry1packet.bottom.lightSensorReading_mV
            unit: "mV"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Input current"
            value: telemetry1packet.bottom.inputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Input voltage"
            value: telemetry1packet.bottom.inputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Output current"
            value: telemetry1packet.bottom.outputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Bottom solar panel"
            description: "Output voltage"
            value: telemetry1packet.bottom.outputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        // ========== Front
        TelemetryObject {
            group: "Front solar panel"
            description: "ID"
            value: telemetry1packet.front.status.solarPanelId
            min: 1
            max: 6
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Antenna status"
            content: (function() {
                if (telemetry1packet.front.status.antennaStatus === "0")
                    return "Not deployed"
                else if (telemetry1packet.front.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Timestamp"
            value: telemetry1packet.front.timestamp
            min: 1
            content: formatDate(telemetry1packet.front.timestamp)
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Temperature"
            value: telemetry1packet.front.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Light sensor"
            value: telemetry1packet.front.lightSensorReading_mV
            unit: "mV"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Input current"
            value: telemetry1packet.front.inputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Input voltage"
            value: telemetry1packet.front.inputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Output current"
            value: telemetry1packet.front.outputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Front solar panel"
            description: "Output voltage"
            value: telemetry1packet.front.outputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        // ========== Right
        TelemetryObject {
            group: "Right solar panel"
            description: "ID"
            value: telemetry1packet.right.status.solarPanelId
            min: 1
            max: 6
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Antenna status"
            content: (function() {
                if (telemetry1packet.right.status.antennaStatus === "0")
                    return "Not deployed"
                else if (telemetry1packet.right.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Timestamp"
            value: telemetry1packet.right.timestamp
            min: 1
            content: formatDate(telemetry1packet.right.timestamp)
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Temperature"
            value: telemetry1packet.right.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Light sensor"
            value: telemetry1packet.right.lightSensorReading_mV
            unit: "mV"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Input current"
            value: telemetry1packet.right.inputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Input voltage"
            value: telemetry1packet.right.inputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Output current"
            value: telemetry1packet.right.outputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Right solar panel"
            description: "Output voltage"
            value: telemetry1packet.right.outputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        // ========== Left
        TelemetryObject {
            group: "Left solar panel"
            description: "ID"
            value: telemetry1packet.left.status.solarPanelId
            min: 1
            max: 6
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Antenna status"
            content: (function() {
                if (telemetry1packet.left.status.antennaStatus === "0")
                    return "Not deployed"
                else if (telemetry1packet.left.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Timestamp"
            value: telemetry1packet.left.timestamp
            min: 1
            content: formatDate(telemetry1packet.left.timestamp)
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Temperature"
            value: telemetry1packet.left.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Light sensor"
            value: telemetry1packet.left.lightSensorReading_mV
            unit: "mV"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Input current"
            value: telemetry1packet.left.inputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Input voltage"
            value: telemetry1packet.left.inputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Output current"
            value: telemetry1packet.left.outputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Left solar panel"
            description: "Output voltage"
            value: telemetry1packet.left.outputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        // ========== Back
        TelemetryObject {
            group: "Back solar panel"
            description: "ID"
            value: telemetry1packet.back.status.solarPanelId
            min: 1
            max: 6
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Antenna status"
            content: (function() {
                if (telemetry1packet.back.status.antennaStatus === "0")
                    return "Not deployed"
                else if (telemetry1packet.back.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Timestamp"
            value: telemetry1packet.back.timestamp
            min: 1
            content: formatDate(telemetry1packet.back.timestamp)
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Temperature"
            value: telemetry1packet.back.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Light sensor"
            value: telemetry1packet.back.lightSensorReading_mV
            unit: "mV"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Input current"
            value: telemetry1packet.back.inputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Input voltage"
            value: telemetry1packet.back.inputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Output current"
            value: telemetry1packet.back.outputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Back solar panel"
            description: "Output voltage"
            value: telemetry1packet.back.outputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        // ========== Top
        TelemetryObject {
            group: "Top solar panel"
            description: "ID"
            value: telemetry1packet.top.status.solarPanelId
            min: 1
            max: 6
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Antenna status"
            content: (function() {
                if (telemetry1packet.top.status.antennaStatus === "0")
                    return "Not deployed"
                else if (telemetry1packet.top.status.antennaStatus === "1")
                    return "Deployed"
                else
                    return "Not monitored"
            })()
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Timestamp"
            value: telemetry1packet.top.timestamp
            min: 1
            content: formatDate(telemetry1packet.top.timestamp)
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Temperature"
            value: telemetry1packet.top.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Light sensor"
            value: telemetry1packet.top.lightSensorReading_mV
            unit: "mV"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Input current"
            value: telemetry1packet.top.inputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Input voltage"
            value: telemetry1packet.top.inputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Output current"
            value: telemetry1packet.top.outputCurrent_mA
            min: 0
            max: 300
            unit: "mA"
        },
        TelemetryObject {
            group: "Top solar panel"
            description: "Output voltage"
            value: telemetry1packet.top.outputVoltage_mV
            min: 0
            max: 5500
            unit: "mV"
        }
    ]
}
