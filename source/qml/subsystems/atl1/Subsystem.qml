
import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../packets"

PacketDetailsTable {
    property string title
    property string shortName
    property var telemetry1packet: mainWindow.lastTelemetry1packet_atl1
    property var telemetry2packet: mainWindow.lastTelemetry2packet_atl1
    property var telemetry3packet: mainWindow.lastTelemetry3packet_atl1
    property var telemetry4packet: mainWindow.lastTelemetry4packet_atl1
    property var telemetry5packet: mainWindow.lastTelemetry5packet_atl1
    property bool valuesOk: true

    onTelemetry1packetChanged: refreshOk()
    onTelemetry2packetChanged: refreshOk()
    onTelemetry3packetChanged: refreshOk()
    onTelemetry4packetChanged: refreshOk()
    onTelemetry5packetChanged: refreshOk()

    function refreshOk() {
        for (var i = 0; i < model.length; i++) {
            var obj = model[i]
            if (!obj.ok) {
                if (valuesOk) {
                    valuesOk = false
                }
                return
            }
        }

        if (!valuesOk) {
            valuesOk = true
        }
    }
}
