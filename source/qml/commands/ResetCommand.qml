
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
        packet.resetType = resetTypeModel.get(resetTypeCombo.currentIndex).value
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Reset command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command tells the satellite to reset the specified unit."
    }

    GenericOptions {
        id: options
    }

    Row {
        spacing: parent.spacing
        width: parent.width

        Label {
            width: leftColumnWidth
            text: "Reset what?"
            height: Math.max(resetTypeCombo.height, implicitHeight)
            anchors.verticalCenter: parent.verticalCenter
        }

        ComboBox {
            id: resetTypeCombo
            anchors.verticalCenter: parent.verticalCenter
            textRole: "text"
            currentIndex: Number(packet.resetType)
            model: ListModel {
                id: resetTypeModel

                ListElement {
                    text: "None"
                    value: OBC.UplinkResetType_None
                    info: "Doesn't reset anything."
                }
                ListElement {
                    text: "PCU"
                    value: OBC.UplinkResetType_PCU
                    info: "Resets both power control units (PCU), which will effectively reboot the whole satellite."
                }
                ListElement {
                    text: "OBC"
                    value: OBC.UplinkResetType_OBC
                    info: "Resets the current on-board computer (OBC)."
                }
                ListElement {
                    text: "COM"
                    value: OBC.UplinkResetType_COM
                    info: "Resets the current communication subsystem (COM)."
                }
                ListElement {
                    text: "TID"
                    value: OBC.UplinkResetType_TID
                    info: "Resets both total ionizing dose sensors (TID)."
                }
                ListElement {
                    text: "OBC user data"
                    value: OBC.UplinkResetType_OBC_UserData
                    info: "Resets the current on-board computer (OBC) user data."
                }
                ListElement {
                    text: "Exit silent mode"
                    value: OBC.UplinkResetType_ExitSilentMode
                    info: "Commands the OBC to exit silent mode."
                }
            }
        }
    }

    Label {
        x: resetTypeCombo.x
        width: parent.width - x
        wrapMode: Text.Wrap
        text: resetTypeModel.get(resetTypeCombo.currentIndex).info
        textFormat: Text.RichText
    }
}
