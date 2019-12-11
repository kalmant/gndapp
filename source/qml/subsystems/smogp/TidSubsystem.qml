
import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../packets"

Subsystem {
    shortName: "TID"
    title: "Latest data about the total ionizing dose sensor (TID)"
    model: [
        TelemetryObject {
            group: "TID1"
            description: "Timestamp"
            value: telemetry3packet.tid1.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "TID1"
            description: "Temperature"
            value: telemetry3packet.tid1.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "TID1"
            description: "Supply voltage"
            value: telemetry3packet.tid1.supplyVoltage_mV
            // TODO: min-max?
            min: 2000
            max: 5000
            unit: "mV"
        },
        TelemetryObject {
            group: "TID1"
            description: "RADFET1"
            value: telemetry3packet.tid1.radfet1_uV
            min: 1
            unit: "uV"
        },
        TelemetryObject {
            group: "TID1"
            description: "RADFET2"
            value: telemetry3packet.tid1.radfet2_uV
            min: 1
            unit: "uV"
        },
        TelemetryObject {
            group: "TID1"
            description: "Measurement no."
            value: telemetry3packet.tid1.measurementSerial
            min: 1
        },


        TelemetryObject {
            group: "TID2"
            description: "Timestamp"
            value: telemetry3packet.tid2.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "TID2"
            description: "Temperature"
            value: telemetry3packet.tid2.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "TID2"
            description: "Supply voltage"
            value: telemetry3packet.tid2.supplyVoltage_mV
            // TODO: min-max?
            min: 2000
            max: 5000
            unit: "mV"
        },
        TelemetryObject {
            group: "TID2"
            description: "RADFET1"
            value: telemetry3packet.tid2.radfet1_uV
            min: 1
            unit: "uV"
        },
        TelemetryObject {
            group: "TID2"
            description: "RADFET2"
            value: telemetry3packet.tid2.radfet2_uV
            min: 1
            unit: "uV"
        },
        TelemetryObject {
            group: "TID2"
            description: "Measurement no."
            value: telemetry3packet.tid2.measurementSerial
            min: 1
        }
    ]
}
