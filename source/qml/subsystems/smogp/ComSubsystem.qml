
import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../packets"

Subsystem {
    shortName: "COM"
    title: "Latest data about the communication subsystem (COM)"
    model: [
        // COM subsystem
        TelemetryObject {
            group: "COM"
            description: "Timestamp"
            value: telemetry3packet.com.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "COM"
            description: "SWR bridge"
            value: telemetry3packet.com.swrBridgeReading
            // TODO: unit?
        },
        TelemetryObject {
            group: "COM"
            description: "Last RX RSSI"
            value: telemetry3packet.com.lastReceivedRssi
            unit: "dBm"
        },
        TelemetryObject {
            group: "COM"
            description: "Supply voltage"
            value: telemetry3packet.com.activeComVoltage_mV
            min: 2200
            max: 3500
            unit: "mV"
        },
        TelemetryObject {
            group: "COM"
            description: "Temperature"
            value: telemetry3packet.com.activeComTemperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        TelemetryObject {
            group: "COM"
            description: "SPA works"
            value: telemetry3packet.com.spectrumAnalyzerStatus.works
            min: 1
            content: telemetry3packet.com.spectrumAnalyzerStatus.works ? "OK" : "NO"
        },
        TelemetryObject {
            group: "COM"
            description: "SPA temperature"
            value: telemetry3packet.com.activeComSpectrumAnalyzerTemperature_C10 / 10.0
            min: -40
            max: 80
            unit: "°C"
        },
        // OBC subsystem
        TelemetryObject {
            group: "OBC"
            description: "Timestamp"
            value: telemetry3packet.timestamp
            min: 1
            content: formatDate(value)
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
            description: "COM uptime"
            content: String(telemetry4packet.diag.comUptimeMin) + " min"
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
        }
    ]
}
