
import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../packets"

Subsystem {
    shortName: "BATT"
    title: "Latest data about the battery panel (BATT)"
    model: [
        // Panel 1
        TelemetryObject {
            group: "P1"
            description: "Timestamp"
            value: telemetry5packet.panel1.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "P1"
            description: "Valid"
            value: telemetry5packet.panel1.valid
        },
        TelemetryObject {
            group: "P1"
            description: "One-wire bus"
            value: telemetry5packet.panel1.oneWireBus
            min: 0
            content: value ? "2" : "1"
        },
        TelemetryObject {
            group: "P1"
            description: "Battery current"
            value: telemetry5packet.panel1.current_mA[0]
            unit: " mA"
        },
        TelemetryObject {
            group: "P1"
            description: "Temperature 1 (Right)"
            value: telemetry5packet.panel1.temperature1_mC[0]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P1"
            description: "Temperature 2 (Top)"
            value: telemetry5packet.panel1.temperature2_mC[0]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P1"
            description: "Temperature 3 (Left)"
            value: telemetry5packet.panel1.temperature3_mC[0]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P1"
            description: "Temperature 4 (Back)"
            value: telemetry5packet.panel1.temperature4_mC[0]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P1"
            description: "Temperature 5 (Panel)"
            value: telemetry5packet.panel1.temperature5_mC[0]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        // Panel 2
        TelemetryObject {
            group: "P2"
            description: "Timestamp"
            value: telemetry5packet.panel2.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "P2"
            description: "Valid"
            value: telemetry5packet.panel2.valid
        },
        TelemetryObject {
            group: "P2"
            description: "One-wire bus"
            value: telemetry5packet.panel2.oneWireBus
            min: 0
            content: value ? "2" : "1"
        },
        TelemetryObject {
            group: "P2"
            description: "Battery current"
            value: telemetry5packet.panel2.current_mA[1]
            unit: " mA"
        },
        TelemetryObject {
            group: "P2"
            description: "Temperature 1 (Right)"
            value: telemetry5packet.panel2.temperature1_mC[1]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P2"
            description: "Temperature 2 (Top)"
            value: telemetry5packet.panel2.temperature2_mC[1]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P2"
            description: "Temperature 3 (Left)"
            value: telemetry5packet.panel2.temperature3_mC[1]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P2"
            description: "Temperature 4 (Back)"
            value: telemetry5packet.panel2.temperature4_mC[1]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P2"
            description: "Temperature 5 (Panel)"
            value: telemetry5packet.panel2.temperature5_mC[1]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        // Panel 3
        TelemetryObject {
            group: "P3"
            description: "Timestamp"
            value: telemetry5packet.panel3.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "P3"
            description: "Valid"
            value: telemetry5packet.panel3.valid
        },
        TelemetryObject {
            group: "P3"
            description: "One-wire bus"
            value: telemetry5packet.panel3.oneWireBus
            min: 0
            content: value ? "2" : "1"
        },
        TelemetryObject {
            group: "P3"
            description: "Battery current"
            value: telemetry5packet.panel3.current_mA[2]
            min: 1
            unit: " mA"
        },
        TelemetryObject {
            group: "P3"
            description: "Temperature 1 (Right)"
            value: telemetry5packet.panel3.temperature1_mC[2]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P3"
            description: "Temperature 2 (Top)"
            value: telemetry5packet.panel3.temperature2_mC[2]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P3"
            description: "Temperature 3 (Left)"
            value: telemetry5packet.panel3.temperature3_mC[2]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P3"
            description: "Temperature 4 (Back)"
            value: telemetry5packet.panel3.temperature4_mC[2]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P3"
            description: "Temperature 5 (Panel)"
            value: telemetry5packet.panel3.temperature5_mC[2]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        // Panel 4
        TelemetryObject {
            group: "P4"
            description: "Timestamp"
            value: telemetry5packet.panel4.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "P4"
            description: "Valid"
            value: telemetry5packet.panel4.valid
        },
        TelemetryObject {
            group: "P4"
            description: "One-wire bus"
            value: telemetry5packet.panel4.oneWireBus
            min: 0
            content: value ? "2" : "1"
        },
        TelemetryObject {
            group: "P4"
            description: "Battery current"
            value: telemetry5packet.panel4.current_mA[3]
            min: 1
            unit: " mA"
        },
        TelemetryObject {
            group: "P4"
            description: "Temperature 1 (Right)"
            value: telemetry5packet.panel4.temperature1_mC[3]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P4"
            description: "Temperature 2 (Top)"
            value: telemetry5packet.panel4.temperature2_mC[3]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P4"
            description: "Temperature 3 (Left)"
            value: telemetry5packet.panel4.temperature3_mC[3]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P4"
            description: "Temperature 4 (Back)"
            value: telemetry5packet.panel4.temperature4_mC[3]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        },
        TelemetryObject {
            group: "P4"
            description: "Temperature 5 (Panel)"
            value: telemetry5packet.panel4.temperature5_mC[3]
            min: -40000
            max: 80000
            content: String(value / 1000.0) + " °C"
        }
    ]
}
