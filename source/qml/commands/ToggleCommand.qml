import QtQuick 2.9
import QtQuick.Controls 2.2
import s1obc 1.0

Column {
    id: view

    property var packet
    property alias when: options.when
    property alias repeat: options.repeat

    spacing: 10
    width: 640

    function updatePacket() {
        packet.toggleType = toggleTypeModel.get(toggleTypeCombo.currentIndex).value
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Toggle command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command tells the satellite to toggle between the redundant pairs of the specified unit."
    }

    GenericOptions {
        id: options
    }

    Row {
        spacing: 10
        width: parent.width

        Label {
            width: leftColumnWidth
            text: "Toggle what?"
            height: Math.max(toggleTypeCombo.height, implicitHeight)
            anchors.verticalCenter: parent.verticalCenter
        }

        ComboBox {
            id: toggleTypeCombo
            anchors.verticalCenter: parent.verticalCenter
            textRole: "text"
            currentIndex: Number(packet.toggleType)
            model: ListModel {
                id: toggleTypeModel

                ListElement {
                    text: "None"
                    value: OBC.UplinkToggleType_None
                    info: "Doesn't toggle anything."
                }
                ListElement {
                    text: "SDC"
                    value: OBC.UplinkToggleType_SDC
                    info: "Toggles the current step-down converter (SDC) chain."
                }
                ListElement {
                    text: "OBC"
                    value: OBC.UplinkToggleType_OBC
                    info: "Toggles the current on-board computer (OBC)."
                }
                ListElement {
                    text: "COM"
                    value: OBC.UplinkToggleType_COM
                    info: "Toggles the current communication subsystem (COM)."
                }
                ListElement {
                    text: "Battery discharge"
                    value: OBC.UplinkToggleType_BatteryDischarge
                    info: "Toggles whether battery discharge is allowed."
                }
                ListElement {
                    text: "Battery charge"
                    value: OBC.UplinkToggleType_BatteryCharge
                    info: "Toggles whether battery charge is allowed."
                }
                ListElement {
                    text: "Flash"
                    value: OBC.UplinkToggleType_Flash
                    info: "Toggles the currently used flash chip."
                }
            }
        }
    }

    Label {
        x: toggleTypeCombo.x
        width: parent.width - x
        wrapMode: Text.Wrap
        text: toggleTypeModel.get(toggleTypeCombo.currentIndex).info
    }
}
