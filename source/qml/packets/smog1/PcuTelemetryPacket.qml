
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
        // PCU1 deployment
        TelemetryObject {
            group: "PCU1-D"
            description: "Timestamp"
            content: formatDate(packet.deployment1.timestamp)
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Uptime"
            content: String(packet.deployment1.pcuUptimeMinutes) + " min"
                     + " (" + String(packet.deployment1.pcuBootCounter) + " boots)"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Deployment SW 1"
            content: packet.deployment1.status.switch1 ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Deployment SW 2"
            content: packet.deployment1.status.switch2 ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "RBF"
            content: packet.deployment1.status.removeBeforeFlight ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Deployment flag"
            content: String(packet.deployment1.status.pcuDeploymentFlag)
        },
        TelemetryObject {
            group: "PCU1-D"
            description: "Antenna flag"
            content: String(packet.deployment1.status.pcuAntennaFlag)
        },
        // PCU2 deployment
        TelemetryObject {
            group: "PCU2-D"
            description: "Timestamp"
            content: formatDate(packet.deployment2.timestamp)
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Uptime"
            content: String(packet.deployment2.pcuUptimeMinutes) + " min"
                     + " (" + String(packet.deployment2.pcuBootCounter) + " boots)"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Deployment SW 1"
            content: packet.deployment2.status.switch1 ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Deployment SW 2"
            content: packet.deployment2.status.switch2 ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "RBF"
            content: packet.deployment2.status.removeBeforeFlight ? "ON" : "OFF"
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Deployment flag"
            content: String(packet.deployment2.status.pcuDeploymentFlag)
        },
        TelemetryObject {
            group: "PCU2-D"
            description: "Antenna flag"
            content: String(packet.deployment2.status.pcuAntennaFlag)
        },
        // PCU1 SDC
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Timestamp"
            content:  formatDate(packet.sdc1.timestamp)
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 input current"
            content: String(packet.sdc1.chain1InputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 output current"
            content: String(packet.sdc1.chain1OutputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 output voltage"
            content: String(packet.sdc1.chain1OutputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain1 status"
            content: "OC=" + String(packet.sdc1.status.chain1Overcurrent)
                     + " OV=" + String(packet.sdc1.status.chain1Overvoltage)
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 input current"
            content: String(packet.sdc1.chain2InputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 output current"
            content: String(packet.sdc1.chain2OutputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 output voltage"
            content: String(packet.sdc1.chain2OutputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU1-SDC"
            description: "Chain2 status"
            content: "OC=" + String(packet.sdc1.status.chain2Overcurrent)
                     + "; OV=" + String(packet.sdc1.status.chain2Overvoltage)
        },
        // PCU2 SDC
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Timestamp"
            content: formatDate(packet.sdc2.timestamp)
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 input current"
            content: String(packet.sdc2.chain1InputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 output current"
            content: String(packet.sdc2.chain1OutputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 output voltage"
            content: String(packet.sdc2.chain1OutputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain1 status"
            content: "OC=" + String(packet.sdc2.status.chain1Overcurrent)
                     + "; OV=" + String(packet.sdc2.status.chain1Overvoltage)
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 input current"
            content: String(packet.sdc1.chain2InputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 output current"
            content: String(packet.sdc1.chain2OutputCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 output voltage"
            content: String(packet.sdc1.chain2OutputVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU2-SDC"
            description: "Chain2 status"
            content: "OC=" + String(packet.sdc1.status.chain2Overcurrent)
                     + "; OV=" + String(packet.sdc1.status.chain2Overvoltage)
        },
        // PCU1 Battery
        TelemetryObject {
            group: "PCU1-B"
            description: "Timestamp"
            content: formatDate(packet.battery1.timestamp)
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Voltage"
            content: String(packet.battery1.voltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Charge current"
            content: String(packet.battery1.chargeCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Discharge current"
            content: String(packet.battery1.dischargeCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Charge status"
            content: (packet.battery1.status.isChargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + packet.battery1.status.chargeOvercurrent
                     + "; OV="+ packet.battery1.status.chargeOvervoltage + ")"
        },
        TelemetryObject {
            group: "PCU1-B"
            description: "Discharge status"
            content: (packet.battery1.status.isDischargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + packet.battery1.status.dischargeOvercurrent
                     + "; OV="+ packet.battery1.status.dischargeOvervoltage + ")"
        },
        // PCU2 Battery
        TelemetryObject {
            group: "PCU2-B"
            description: "Timestamp"
            content: formatDate(packet.battery2.timestamp)
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Voltage"
            content: String(packet.battery2.voltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Charge current"
            content: String(packet.battery2.chargeCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Discharge current"
            content: String(packet.battery2.dischargeCurrent_mA) + " mA"
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Charge status"
            content: (packet.battery2.status.isChargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + packet.battery2.status.chargeOvercurrent
                     + "; OV="+ packet.battery2.status.chargeOvervoltage + ")"
        },
        TelemetryObject {
            group: "PCU2-B"
            description: "Discharge status"
            content: (packet.battery2.status.isDischargeEnabled ? "Enabled" : "Disabled")
                     + " (OC=" + packet.battery2.status.dischargeOvercurrent
                     + "; OV="+ packet.battery2.status.dischargeOvervoltage + ")"
        },
        // PCU1 Bus
        TelemetryObject {
            group: "PCU1-Bus"
            description: "Timestamp"
            content: formatDate(packet.bus1.timestamp)
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "Unreg. bus voltage"
            content: String(packet.bus1.unregulatedBusVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "Reg. bus voltage"
            content: String(packet.bus1.regulatedBusVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "OBC1 consumption"
            content: String(packet.bus1.obc1CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(packet.bus1.status.obc1Overcurrent) + ")"
        },
        TelemetryObject {
            group: "PCU1-Bus"
            description: "OBC2 consumption"
            content: String(packet.bus1.obc2CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(packet.bus1.status.obc2Overcurrent) + ")"
        },
        // PCU2 Bus
        TelemetryObject {
            group: "PCU2-Bus"
            description: "Timestamp"
            content: formatDate(packet.bus2.timestamp)
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "Unreg. bus voltage"
            content: String(packet.bus2.unregulatedBusVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "Reg. bus voltage"
            content: String(packet.bus2.regulatedBusVoltage_mV) + " mV"
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "OBC1 consumption"
            content: String(packet.bus2.obc1CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(packet.bus2.status.obc1Overcurrent) + ")"
        },
        TelemetryObject {
            group: "PCU2-Bus"
            description: "OBC2 consumption"
            content: String(packet.bus2.obc2CurrentConsumption_mA) + " mA"
                     + " (OC=" + String(packet.bus2.status.obc2Overcurrent) + ")"
        },
        // PCU Voltage
        TelemetryObject {
            group: "PCU-V"
            description: "PCU1 voltage"
            content: String(packet.pcu1Voltage) + " mV"
        },
        TelemetryObject {
            group: "PCU-V"
            description: "PCU2 voltage"
            content: String(packet.pcu2Voltage) + " mV"
        }
    ]
}
