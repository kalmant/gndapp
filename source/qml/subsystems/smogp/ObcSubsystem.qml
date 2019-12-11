
import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../packets"

Subsystem {
    shortName: "OBC"
    title: "Latest data about the on-board computer (OBC)"
    model: [
        TelemetryObject {
            group: "OBC"
            description: "Timestamp"
            value: telemetry3packet.timestamp
            min: 1
            content: formatDate(telemetry3packet.timestamp)
        },
        TelemetryObject {
            group: "OBC"
            description: "MCU voltage"
            value: telemetry3packet.obc.supplyVoltage_mV
            min: 1800
            max: 3500
            unit: "mV"
        },
        TelemetryObject {
            group: "OBC"
            description: "RTCC temperature"
            value: telemetry3packet.obc.rtccTemperature_C
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "OBC"
            description: "EPS2-A T1"
            value: telemetry3packet.obc.eps2PanelATemperature1_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "OBC"
            description: "EPS2-A T2"
            value: telemetry3packet.obc.eps2PanelATemperature1_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "OBC"
            description: "COM TX current (avg)"
            value: telemetry3packet.obc.comTxCurrent_mA
            min: 15
            max: 190
            unit: "mA"
        },
        TelemetryObject {
            group: "OBC"
            description: "COM RX current (avg)"
            value: telemetry3packet.obc.comRxCurrent_mA
            min: 10
            max: 50
            unit: "mA"
        },
        TelemetryObject {
            group: "OBC"
            description: "COM TX data rate"
            content: String(telemetry3packet.obc.comTxStatus.dataRate)
        },
        TelemetryObject {
            group: "OBC"
            description: "COM1 limiter"
            content: "SW=" + String(telemetry3packet.obc.comLimiterStatus.com1LimiterSwitch)
                     + "; OC=" + String(telemetry3packet.obc.comLimiterStatus.com1LimiterOvercurrent)
                     + "; OVR=" + String(telemetry3packet.obc.comLimiterStatus.com1LimiterOverride)
            ok: ((telemetry3packet.obc.comLimiterStatus.com1LimiterSwitch && !telemetry3packet.obc.comLimiterStatus.com2LimiterSwitch)
                 || (!telemetry3packet.obc.comLimiterStatus.com1LimiterSwitch && telemetry3packet.obc.comLimiterStatus.com2LimiterSwitch))
                && !telemetry3packet.obc.comLimiterStatus.com1LimiterOvercurrent
                && !telemetry3packet.obc.comLimiterStatus.com1LimiterOverride
        },
        TelemetryObject {
            group: "OBC"
            description: "COM2 limiter"
            content: "SW=" + String(telemetry3packet.obc.comLimiterStatus.com2LimiterSwitch)
                     + "; OC=" + String(telemetry3packet.obc.comLimiterStatus.com2LimiterOvercurrent)
                     + "; OVR=" + String(telemetry3packet.obc.comLimiterStatus.com2LimiterOverride)
            ok: ((telemetry3packet.obc.comLimiterStatus.com1LimiterSwitch && !telemetry3packet.obc.comLimiterStatus.com2LimiterSwitch)
                 || (!telemetry3packet.obc.comLimiterStatus.com1LimiterSwitch && telemetry3packet.obc.comLimiterStatus.com2LimiterSwitch))
                && !telemetry3packet.obc.comLimiterStatus.com2LimiterOvercurrent
                && !telemetry3packet.obc.comLimiterStatus.com2LimiterOverride
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU gyroscope"
            content: "x=" + String(telemetry3packet.obc.motionSensor.gyroscope.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor.gyroscope.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor.gyroscope.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU magnetometer"
            content: "x=" + String(telemetry3packet.obc.motionSensor.magnetometer.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor.magnetometer.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor.magnetometer.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU accelerometer"
            content: "x=" + String(telemetry3packet.obc.motionSensor.accelerometer.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor.accelerometer.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor.accelerometer.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU temperature"
            value: telemetry3packet.obc.motionSensor.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "OBC"
            description: "Active OBC/COM"
            content: "OBC" + String(telemetry3packet.obc.obcBoardStatus.activeObc + 1)
                     + " | COM" + String(telemetry3packet.obc.obcBoardStatus.activeCom + 1)
        },
        TelemetryObject {
            group: "OBC"
            description: "Flash status"
            value: telemetry3packet.obc.obcBoardStatus.flashStatus
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.flashStatus ? "OK" : "NO"
        },
        TelemetryObject {
            group: "OBC"
            description: "RTCC status"
            value: telemetry3packet.obc.obcBoardStatus.rtccStatus
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.rtccStatus ? "OK" : "NO"
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU status"
            value: telemetry3packet.obc.obcBoardStatus.motionSensorStatus
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.motionSensorStatus ? "OK" : "NO"
        },
        // ========== Diagnostic info
        TelemetryObject {
            group: "OBC"
            description: "Valid packets"
            content: String(telemetry4packet.diag.validPackets)
        },
        TelemetryObject {
            group: "OBC"
            description: "Wrong sized packets"
            content: String(telemetry4packet.diag.wrongSizedPackets)
        },
        TelemetryObject {
            group: "OBC"
            description: "Packets w/ failed Golay"
            content: String(telemetry4packet.diag.packetsWithFailedGolayDecoding)
        },
        TelemetryObject {
            group: "OBC"
            description: "Packets w/ wrong signature"
            content: String(telemetry4packet.diag.packetsWithWrongSignature)
        },
        TelemetryObject {
            group: "OBC"
            description: "Packets w/ invalid serial"
            content: String(telemetry4packet.diag.packetsWithInvalidSerialNumber)
        },
        TelemetryObject {
            group: "OBC"
            description: "OBC-COM UART errors"
            content: String(telemetry4packet.diag.uartErrorCounter)
        },
        // PCU1 Bus
        TelemetryObject {
            group: "PCU1-Bus"
            description: "Timestamp"
            value: telemetry2packet.bus1.timestamp
            min: 1
            content: formatDate(telemetry2packet.bus1.timestamp)
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "OBC1 consumption"
            value: telemetry2packet.bus1.obc1CurrentConsumption_mA
            max: 40
            content: String(telemetry2packet.bus1.obc1CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus1.status.obc1Overcurrent) + ")"
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "OBC2 consumption"
            value: telemetry2packet.bus1.obc2CurrentConsumption_mA
            max: 40
            content: String(telemetry2packet.bus1.obc2CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus1.status.obc2Overcurrent) + ")"
        },
        // PCU2 Bus
        TelemetryObject {
            group: "PCU2-Bus"
            description: "Timestamp"
            value: telemetry2packet.bus2.timestamp
            min: 1
            content: formatDate(telemetry2packet.bus2.timestamp)
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "OBC1 consumption"
            value: telemetry2packet.bus2.obc1CurrentConsumption_mA
            max: 40
            content: String(telemetry2packet.bus2.obc1CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus2.status.obc1Overcurrent) + ")"
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "OBC2 consumption"
            value: telemetry2packet.bus2.obc2CurrentConsumption_mA
            max: 40
            content: String(telemetry2packet.bus2.obc2CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus2.status.obc2Overcurrent) + ")"
        }
    ]
}
