
import QtQuick 2.9
import QtQuick.Controls 2.2
import s1obc 1.0
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
        TelemetryObject {
            group: "OBC"
            description: "MCU voltage"
            content: String(packet.obc.supplyVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "OBC"
            description: "RTCC temperature"
            content: String(packet.obc.rtccTemperature_C) + " °C"
        },
        TelemetryObject {
            group: "OBC"
            description: "EPS2-A T1"
            content: String(packet.obc.eps2PanelATemperature1_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "OBC"
            description: "EPS2-A T2"
            content: String(packet.obc.eps2PanelATemperature1_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "OBC"
            description: "COM TX current (avg)"
            content: String(packet.obc.comTxCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "OBC"
            description: "COM RX current (avg)"
            content: String(packet.obc.comRxCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "OBC"
            description: "COM TX data rate"
            content: {
                switch (packet.obc.comTxStatus.dataRate) {
                case OBC.ComTxDatarate_500bps:
                    return "500 bps"
                case OBC.ComTxDatarate_1250bps:
                    return "1250 bps"
                case OBC.ComTxDatarate_2500bps:
                    return "2500 bps"
                case OBC.ComTxDatarate_5000bps:
                    return "5000 bps"
                case OBC.ComTxDatarate_12500bps:
                    return "12500 bps"
                case OBC.ComTxDatarate_25000bps:
                    return "25000 bps"
                case OBC.ComTxDatarate_50000bps:
                    return "50000 bps"
                case OBC.ComTxDatarate_125000bps:
                    return "125000 bps"
                }

                return "unknown"
            }
        },
        TelemetryObject {
            group: "OBC"
            description: "COM1 limiter"
            content: "SW=" + String(packet.obc.comLimiterStatus.com1LimiterSwitch)
                     + " OC=" + String(packet.obc.comLimiterStatus.com1LimiterOvercurrent)
                     + " OVR=" + String(packet.obc.comLimiterStatus.com1LimiterOverride)
        },
        TelemetryObject {
            group: "OBC"
            description: "COM2 limiter"
            content: "SW=" + String(packet.obc.comLimiterStatus.com2LimiterSwitch)
                     + " OC=" + String(packet.obc.comLimiterStatus.com2LimiterOvercurrent)
                     + " OVR=" + String(packet.obc.comLimiterStatus.com2LimiterOverride)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU gyroscope"
            content: "x=" + String(packet.obc.motionSensor.gyroscope.x)
                     + "; y=" + String(packet.obc.motionSensor.gyroscope.y)
                     + "; z=" + String(packet.obc.motionSensor.gyroscope.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU gyroscope"
            content: "x=" + String(packet.obc.motionSensor.gyroscope.x)
                     + "; y=" + String(packet.obc.motionSensor.gyroscope.y)
                     + "; z=" + String(packet.obc.motionSensor.gyroscope.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU magnetometer"
            content: "x=" + String(packet.obc.motionSensor.magnetometer.x)
                     + "; y=" + String(packet.obc.motionSensor.magnetometer.y)
                     + "; z=" + String(packet.obc.motionSensor.magnetometer.z)
        },
        TelemetryObject {
            group: "OBC"
            description: "MPU temperature"
            content: String(packet.obc.motionSensor.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "OBC"
            description: "Active OBC/COM"
            content: "OBC" + String(packet.obc.obcBoardStatus.activeObc + 1)
                     + " | COM" + String(packet.obc.obcBoardStatus.activeCom + 1)
        },
        TelemetryObject {
            group: "OBC"
            description: "SPI"
            content: "flash: " + String(packet.obc.obcBoardStatus.flashStatus ? "OK" : "NO")
                     + " mpu: " + String(packet.obc.obcBoardStatus.motionSensorStatus ? "OK" : "NO")
                     + " rtcc: " + String(packet.obc.obcBoardStatus.rtccStatus ? "OK" : "NO")
        },
        TelemetryObject {
            group: "COM"
            description: "Timestamp"
            content: formatDate(packet.com.timestamp)
        },
        TelemetryObject {
            group: "COM"
            description: "SWR bridge"
            content: String(packet.com.swrBridgeReading) // TODO: unit?
        },
        TelemetryObject {
            group: "COM"
            description: "Last RX RSSI"
            content: String(packet.com.lastReceivedRssi) + " dBm"
        },
        TelemetryObject {
            group: "COM"
            description: "Supply voltage"
            content: String(packet.com.activeComVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "COM"
            description: "Temperature"
            content: String(packet.com.activeComTemperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "COM"
            description: "SPA works"
            content: String(packet.com.spectrumAnalyzerStatus.works ? "OK" : "NO")
        },
        TelemetryObject {
            group: "COM"
            description: "SPA temperature"
            content: String(packet.com.activeComSpectrumAnalyzerTemperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "TID1"
            description: "Timestamp"
            content: formatDate(packet.tid1.timestamp)
        },
        TelemetryObject {
            group: "TID1"
            description: "Temperature"
            content: String(packet.tid1.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "TID1"
            description: "Supply voltage"
            content: String(packet.tid1.supplyVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "TID1"
            description: "RADFET1"
            content: String(packet.tid1.radfet1_uV) + " uV"
        },
        TelemetryObject {
            group: "TID1"
            description: "RADFET2"
            content: String(packet.tid1.radfet2_uV) + " uV"
        },
        TelemetryObject {
            group: "TID1"
            description: "Measurement no."
            content: String(packet.tid1.measurementSerial)
        },
        TelemetryObject {
            group: "TID2"
            description: "Timestamp"
            content: formatDate(packet.tid2.timestamp)
        },
        TelemetryObject {
            group: "TID2"
            description: "Temperature"
            content: String(packet.tid2.temperature_C10 / 10.0) + " °C"
        },
        TelemetryObject {
            group: "TID2"
            description: "Supply voltage"
            content: String(packet.tid2.supplyVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "TID2"
            description: "RADFET1"
            content: String(packet.tid2.radfet1_uV) + " uV"
        },
        TelemetryObject {
            group: "TID2"
            description: "RADFET2"
            content: String(packet.tid2.radfet2_uV) + " uV"
        },
        TelemetryObject {
            group: "TID2"
            description: "Measurement no."
            content: String(packet.tid2.measurementSerial)
        }
    ]
}
