
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
            description: "MCU temperature"
            value: telemetry3packet.obc.activeObcTemperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "OBC"
            description: "RTCC1 temperature"
            value: telemetry3packet.obc.rtcc1Temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "OBC"
            description: "RTCC2 temperature"
            value: telemetry3packet.obc.rtcc1Temperature_C10 / 10.0
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
            value: telemetry3packet.obc.eps2PanelATemperature2_C10 / 10.0
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
            description: "COM TX power level"
            content: String(telemetry3packet.obc.comTxStatus.powerLevel)
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
            description: "MPU1 gyroscope"
            content: "x=" + String(telemetry3packet.obc.motionSensor1.gyroscope.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor1.gyroscope.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor1.gyroscope.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU2 gyroscope"
            content: "x=" + String(telemetry3packet.obc.motionSensor2.gyroscope.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor2.gyroscope.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor2.gyroscope.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU1 magnetometer"
            content: "x=" + String(telemetry3packet.obc.motionSensor1.magnetometer.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor1.magnetometer.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor1.magnetometer.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU2 magnetometer"
            content: "x=" + String(telemetry3packet.obc.motionSensor2.magnetometer.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor2.magnetometer.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor2.magnetometer.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU1 accelerometer"
            content: "x=" + String(telemetry3packet.obc.motionSensor1.accelerometer.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor1.accelerometer.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor1.accelerometer.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU2 accelerometer"
            content: "x=" + String(telemetry3packet.obc.motionSensor2.accelerometer.x)
                     + "; y=" + String(telemetry3packet.obc.motionSensor2.accelerometer.y)
                     + "; z=" + String(telemetry3packet.obc.motionSensor2.accelerometer.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU1 temperature"
            value: telemetry3packet.obc.motionSensor1.temperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU2 temperature"
            value: telemetry3packet.obc.motionSensor2.temperature_C10 / 10.0
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
            description: "Flash 1 status"
            value: telemetry3packet.obc.obcBoardStatus.flash1Status
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.flash1Status ? "OK" : "NO"
        },
        TelemetryObject {
            group: "OBC"
            description: "Flash 2 status"
            value: telemetry3packet.obc.obcBoardStatus.flash2Status
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.flash2Status ? "OK" : "NO"
        },
        TelemetryObject {
            group: "OBC"
            description: "RTCC 1 status"
            value: telemetry3packet.obc.obcBoardStatus.rtcc1Status
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.rtcc1Status ? "OK" : "NO"
        },
        TelemetryObject {
            group: "OBC"
            description: "RTCC 2 status"
            value: telemetry3packet.obc.obcBoardStatus.rtcc2Status
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.rtcc2Status ? "OK" : "NO"
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU 1 status"
            value: telemetry3packet.obc.obcBoardStatus.motionSensor1Status
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.motionSensor1Status ? "OK" : "NO"
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU 2 status"
            value: telemetry3packet.obc.obcBoardStatus.motionSensor2Status
            min: 1
            content: telemetry3packet.obc.obcBoardStatus.motionSensor2Status ? "OK" : "NO"
        },
        // ========== Diagnostic info
        TelemetryObject {
            group: "OBC"
            description: "OBC flash checksum"
            content: String(telemetry4packet.diag.flashChecksum)
        },
        TelemetryObject {
            group: "OBC"
            description: "Last uplink timestamp"
            content: formatDate(telemetry4packet.diag.lastUplinkTimestamp)
        },
        TelemetryObject {
            group: "OBC"
            description: "OBC uptime"
            content: String(telemetry4packet.diag.obcUptimeMin) + " min"
        },
        TelemetryObject {
            group: "OBC"
            description: "TX voltage drop"
            content: String(telemetry4packet.diag.txVoltageDrop_10mV * 10.0) + " mV"
        },
        TelemetryObject {
            group: "OBC"
            description: "Timed task count"
            content: String(telemetry4packet.diag.taskCount)
        },
        TelemetryObject {
            group: "OBC"
            description: "Energy mode"
            content: String(telemetry4packet.diag.status.energyMode)
        },
        TelemetryObject {
            group: "OBC"
            description: "TCXO works"
            content: telemetry4packet.diag.status.tcxoWorks ? "OK" : "NO"
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
