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
            description: "MSG1"
            content: packet.messageList[0].content
        },
        TelemetryObject {
            group: "-"
            description: "MSG2"
            content: packet.messageList[1].content
        },
        TelemetryObject {
            group: "-"
            description: "MSG3"
            content: packet.messageList[2].content
        },
        TelemetryObject {
            group: "-"
            description: "MSG4"
            content: packet.messageList[3].content
        },
        TelemetryObject {
            group: "-"
            description: "MSG5"
            content: packet.messageList[4].content
        },
        TelemetryObject {
            group: "-"
            description: "MSG6"
            content: packet.messageList[5].content
        },
        TelemetryObject {
            group: "-"
            description: "MSG7"
            content: packet.messageList[6].content
        },
        TelemetryObject {
            group: "-"
            description: "MSG8"
            content: packet.messageList[7].content
        }
    ]
}
