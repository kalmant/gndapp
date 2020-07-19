
import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../packets"

Subsystem {
    shortName: "PCU"
    title: "Latest data about the power control unit (PCU)"
    model: [
        // PCU1 deployment
        TelemetryObject {
            group: "PCU1-D"
            description: "Timestamp"
            value: telemetry2packet.deployment1.timestamp
            min: 1
            content: formatDate(telemetry2packet.deployment1.timestamp)
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Uptime"
            value: telemetry2packet.deployment1.pcuBootCounter
            min: 1
            content: String(telemetry2packet.deployment1.pcuUptimeMinutes) + " min"
                     + " (" + String(telemetry2packet.deployment1.pcuBootCounter) + " boots)"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Deployment SW 1"
            value: telemetry2packet.deployment1.status.switch1
            min: 1
            content: value ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Deployment SW 2"
            value: telemetry2packet.deployment1.status.switch2
            min: 1
            content: value ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "RBF"
            value: telemetry2packet.deployment1.status.removeBeforeFlight
            min: 1
            content: value ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Deployment flag"
            value: telemetry2packet.deployment1.status.pcuDeploymentFlag
            min: 1
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Antenna flag"
            value: telemetry2packet.deployment1.status.pcuAntennaFlag
            min: 1
        },
        // PCU2 deployment
        TelemetryObject {
            group: "PCU2-D"
            description: "Timestamp"
            value: telemetry2packet.deployment2.timestamp
            min: 1
            content: formatDate(telemetry2packet.deployment2.timestamp)
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Uptime"
            value: telemetry2packet.deployment2.pcuBootCounter
            min: 1
            content: String(telemetry2packet.deployment2.pcuUptimeMinutes) + " min"
                     + " (" + String(telemetry2packet.deployment2.pcuBootCounter) + " boots)"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Deployment SW 1"
            value: telemetry2packet.deployment2.status.switch1
            min: 1
            content: value ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Deployment SW 2"
            value: telemetry2packet.deployment2.status.switch2
            min: 1
            content: value ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "RBF"
            value: telemetry2packet.deployment2.status.removeBeforeFlight
            min: 1
            content: value ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Deployment flag"
            value: telemetry2packet.deployment2.status.pcuDeploymentFlag
            min: 1
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Antenna flag"
            value: telemetry2packet.deployment2.status.pcuAntennaFlag
            min: 1
        },
        // PCU1 SDC
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Timestamp"
            value: telemetry2packet.sdc1.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 input current"
            value: telemetry2packet.sdc1.chain1InputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 output current"
            value: telemetry2packet.sdc1.chain1OutputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 output voltage"
            value: telemetry2packet.sdc1.chain1OutputVoltage_mV
            min: 0
            max: 3500
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 status"
            content: "OC=" + String(telemetry2packet.sdc1.status.chain1Overcurrent)
                     + " OV=" + String(telemetry2packet.sdc1.status.chain1Overvoltage)
            ok: !telemetry2packet.sdc1.status.chain1Overcurrent && !telemetry2packet.sdc1.status.chain1Overvoltage
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 input current"
            value: telemetry2packet.sdc1.chain2InputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 output current"
            value: telemetry2packet.sdc1.chain2OutputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 output voltage"
            value: telemetry2packet.sdc1.chain2OutputVoltage_mV
            min: 0
            max: 3500
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 status"
            content: "OC=" + String(telemetry2packet.sdc1.status.chain2Overcurrent)
                     + "; OV=" + String(telemetry2packet.sdc1.status.chain2Overvoltage)
            ok: !telemetry2packet.sdc1.status.chain2Overcurrent && !telemetry2packet.sdc1.status.chain2Overvoltage
        },
        // PCU2 SDC
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Timestamp"
            value: telemetry2packet.sdc2.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 input current"
            value: telemetry2packet.sdc2.chain1InputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 output current"
            value: telemetry2packet.sdc2.chain1OutputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 output voltage"
            value: telemetry2packet.sdc2.chain1OutputVoltage_mV
            min: 0
            max: 3500
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 status"
            content: "OC=" + String(telemetry2packet.sdc2.status.chain1Overcurrent)
                     + "; OV=" + String(telemetry2packet.sdc2.status.chain1Overvoltage)
            ok: !telemetry2packet.sdc2.status.chain1Overcurrent && !telemetry2packet.sdc2.status.chain1Overvoltage
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 input current"
            value: telemetry2packet.sdc2.chain2InputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 output current"
            value: telemetry2packet.sdc2.chain2OutputCurrent_mA
            min: 0
            max: 250
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 output voltage"
            value: telemetry2packet.sdc2.chain2OutputVoltage_mV
            min: 0
            max: 3500
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 status"
            content: "OC=" + String(telemetry2packet.sdc2.status.chain2Overcurrent)
                     + "; OV=" + String(telemetry2packet.sdc2.status.chain2Overvoltage)
            ok: !telemetry2packet.sdc2.status.chain2Overcurrent && !telemetry2packet.sdc2.status.chain2Overvoltage
        },
        // PCU1 Battery
        TelemetryObject {
            group: "PCU1-B"
            description: "Timestamp"
            value: telemetry2packet.battery1.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Voltage"
            value: telemetry2packet.battery1.voltage_mV
            min: 2300
            max: 5000
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Charge current"
            value: telemetry2packet.battery1.chargeCurrent_mA
            min: 0
            max: 500
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Discharge current"
            value: telemetry2packet.battery1.dischargeCurrent_mA
            min: 0
            max: 500
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Charge status"
            content: (telemetry2packet.battery1.status.isChargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + telemetry2packet.battery1.status.chargeOvercurrent
                     + "; OV="+ telemetry2packet.battery1.status.chargeOvervoltage + ")"
            ok: !telemetry2packet.battery1.status.chargeOvercurrent && !telemetry2packet.battery1.status.chargeOvervoltage
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Discharge status"
            content: (telemetry2packet.battery1.status.isDischargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + telemetry2packet.battery1.status.dischargeOvercurrent
                     + "; OV="+ telemetry2packet.battery1.status.dischargeOvervoltage + ")"
            ok: !telemetry2packet.battery1.status.dischargeOvercurrent && !telemetry2packet.battery1.status.dischargeOvervoltage
        },
        // PCU2 Battery
        TelemetryObject {
            group: "PCU2-B"
            description: "Timestamp"
            value: telemetry2packet.battery2.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Voltage"
            value: telemetry2packet.battery2.voltage_mV
            min: 2300
            max: 5000
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Charge current"
            value: telemetry2packet.battery2.chargeCurrent_mA
            min: 0
            max: 500
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Discharge current"
            value: telemetry2packet.battery2.dischargeCurrent_mA
            min: 0
            max: 500
            unit: "mA"
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Charge status"
            content: (telemetry2packet.battery2.status.isChargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + telemetry2packet.battery2.status.chargeOvercurrent
                     + "; OV="+ telemetry2packet.battery2.status.chargeOvervoltage + ")"
            ok: !telemetry2packet.battery2.status.chargeOvercurrent && !telemetry2packet.battery2.status.chargeOvervoltage
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Discharge status"
            content: (telemetry2packet.battery2.status.isDischargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + telemetry2packet.battery2.status.dischargeOvercurrent
                     + "; OV="+ telemetry2packet.battery2.status.dischargeOvervoltage + ")"
            ok: !telemetry2packet.battery2.status.dischargeOvercurrent && !telemetry2packet.battery2.status.dischargeOvervoltage
        },
        // PCU1 Bus
        TelemetryObject {
            group: "PCU1-Bus"
            description: "Timestamp"
            value: telemetry2packet.bus1.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "Unreg. bus voltage"
            value: telemetry2packet.bus1.unregulatedBusVoltage_mV
            min: 2000
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "Reg. bus voltage"
            value: telemetry2packet.bus1.regulatedBusVoltage_mV
            min: 1800
            max: 3500
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "OBC1 consumption"
            content: String(telemetry2packet.bus1.obc1CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus1.status.obc1Overcurrent) + ")"
            ok: !telemetry2packet.bus1.status.obc1Overcurrent && (telemetry2packet.bus1.obc1CurrentConsumption_mA < 15)
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "OBC2 consumption"
            content: String(telemetry2packet.bus1.obc2CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus1.status.obc2Overcurrent) + ")"
            ok: !telemetry2packet.bus1.status.obc2Overcurrent && (telemetry2packet.bus1.obc2CurrentConsumption_mA < 15)
        },
        // PCU2 Bus
        TelemetryObject {
            group: "PCU2-Bus"
            description: "Timestamp"
            value: telemetry2packet.bus2.timestamp
            min: 1
            content: formatDate(value)
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "Unreg. bus voltage"
            value: telemetry2packet.bus2.unregulatedBusVoltage_mV
            min: 2000
            max: 5500
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "Reg. bus voltage"
            value: telemetry2packet.bus2.regulatedBusVoltage_mV
            min: 1800
            max: 3500
            unit: "mV"
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "OBC1 consumption"
            content: String(telemetry2packet.bus2.obc1CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus2.status.obc1Overcurrent) + ")"
            ok: !telemetry2packet.bus2.status.obc1Overcurrent && (telemetry2packet.bus2.obc1CurrentConsumption_mA < 15)
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "OBC2 consumption"
            content: String(telemetry2packet.bus2.obc2CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(telemetry2packet.bus2.status.obc2Overcurrent) + ")"
            ok: !telemetry2packet.bus2.status.obc2Overcurrent && (telemetry2packet.bus2.obc2CurrentConsumption_mA < 15)
        },
        // PCU Voltage
        TelemetryObject {
            group: "PCU-V"
            description: "PCU1 voltage"
            content: String(telemetry2packet.pcu1Voltage) + " mV"
            // TODO: add correct range for value
        },
        TelemetryObject {
            group: "PCU-V"
            description: "PCU2 voltage"
            content: String(telemetry2packet.pcu2Voltage) + " mV"
            // TODO: add correct range for value
        }
    ]
}
