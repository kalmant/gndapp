
import QtQuick 2.9
import QtQuick.Controls 2.2

ScrollView {
    id: root

    property var packet
    property int packetType
    property var packetDetails
    property list<QtObject> model
    property int descriptionColumnWidth: 180
    property int contentColumnWidth: 220
    property int cellPadding: 2
    property string selectedTimeZone

    function formatDate(timestamp) {
        var d = new Date(new Date(timestamp * 1000) - (new Date().getTimezoneOffset() * -60000))
        return Qt.formatDateTime(d, "yyyy-MM-ddThh:mm:ss UTC")
    }

    function formatHexBytes(buffer) {
        var arr = new Uint8Array(buffer)
        var mapFunc = function (n) { return ("0" + n.toString(16)).slice(-2) }
        return Array.prototype.map.call(arr, mapFunc).join("")
    }

    ListView {
        id: table

        model: root.model
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        delegate: Rectangle {
            width: table.width
            height: delegateContainer.height + cellPadding * 2
            color: "#00000000"

            Row {
                id: delegateContainer
                y: cellPadding

                Label {
                    id: descriptionLabel
                    width: descriptionColumnWidth
                    text: model.description
                }
                Label {
                    id: contentLabel
                    width: contentColumnWidth
                    text: model.content
                }
            }

            Rectangle {
                color: "#44cccccc"
                y: delegateContainer.height + cellPadding
                width: table.width
                height: 1
            }

            states: [
                State {
                    name: "item_not_ok"
                    when: !model.ok

                    PropertyChanges {
                        target: descriptionLabel
                        color: "red"
                    }

                    PropertyChanges {
                        target: contentLabel
                        color: "red"
                    }
                }

            ]
        }

        section.property: "group"
        section.criteria: ViewSection.FullString
        section.delegate: Component {
            Label {
                height: implicitHeight * 1.5
                verticalAlignment: Text.AlignVCenter
                text: {
                    if (section === "-")
                        return "Packet"
                    else if (section === "OBC")
                        return "OBC - On-board computer"
                    else if (section === "COM")
                        return "COM - Communication"
                    else if (section === "TID1")
                        return "TID1 - Total ionizing dose sensor #1"
                    else if (section === "TID2")
                        return "TID2 - Total ionizing dose sensor #2"
                    else if (section === "PCU1-D")
                        return "PCU1 deployment telemetry"
                    else if (section === "PCU2-D")
                        return "PCU2 deployment telemetry"
                    else if (section === "PCU1-SDC")
                        return "PCU1 SDC chain telemetry"
                    else if (section === "PCU2-SDC")
                        return "PCU2 SDC chain telemetry"
                    else if (section === "PCU1-B")
                        return "PCU1 battery telemetry"
                    else if (section === "PCU2-B")
                        return "PCU2 battery telemetry"
                    else if (section === "PCU1-Bus")
                        return "PCU1 bus telemetry"
                    else if (section === "PCU2-Bus")
                        return "PCU2 bus telemetry"
                    else if (section === "Diag")
                        return "Diagnostic info"
                    else if (section === "BATT")
                        return "BATT - Battery panel"
                    else if (section === "P1")
                        return "Panel 1 (A2)"
                    else if (section === "P2")
                        return "Panel 2 (Kapton)"
                    else if (section === "P3")
                        return "Panel 3 (SZ2)"
                    else if (section === "P4")
                        return "Panel 4 (K2)"

                    return section
                }

                font.bold: true
            }
        }
    }
}
