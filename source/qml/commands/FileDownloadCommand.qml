
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
        fileIdBox.focus = false
        offsetBox.focus = false
        sizeBox.focus = false

        var now = (new Date()).valueOf() / 1000
        var reltime = 0
        if (scheduleTimeRow.when > now)
            reltime = scheduleTimeRow.when ? (scheduleTimeRow.when - now) : 0

        if (reltime > 0x00ffffff)
            reltime = 0x00ffffff

        packet.relativeTime = reltime
        packet.fileId = fileIdSwitch.checked ? fileIdBox.value : 0
        packet.name = fileIdSwitch.checked ? "" : fileNameField.text
        packet.offset = !startOffsetSwitch.checked ? offsetBox.value : 0
        packet.shouldDelete = deleteSwitch.checked ? 1 : 0
        packet.length = wholeFileSwitch.checked ? 0 : sizeBox.value
        packet.txDataRate = dataRateModel.get(dataRateCombo.currentIndex).value
    }

    Label {
        font.bold: true
        width: parent.width
        text: "File download request command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "This command requests a file download from the satellite."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Which file?"
            anchors.verticalCenter: parent.verticalCenter
        }

        Switch {
            id: fileIdSwitch
            checked: packet.name === "" || packet.fileId > 0
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            id: fileIdLabel
            text: "Use file ID"
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        x: fileIdLabel.x
        width: parent.width - x
        visible: fileIdSwitch.checked
        spacing: parent.spacing

        Label {
            text: "Specify file ID:"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: fileIdBox
            from: fileIdSwitch.checked ? 1 : 0
            to: 255
            anchors.verticalCenter: parent.verticalCenter
            value: packet.fileId
            editable: true
        }
    }

    Row {
        x: fileIdLabel.x
        width: parent.width - x
        visible: !fileIdSwitch.checked
        spacing: parent.spacing

        Label {
            id: specifyFileNameLabel
            text: "Specify file name:"
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField {
            id: fileNameField
            selectByMouse: true
            width: Math.floor((parent.width - specifyFileNameLabel.width - parent.spacing) * 0.66)
            // TODO: Figure out a way to block users from using an empty filename
            maximumLength: 10
            text: packet.name
        }
    }

    CommandTimeRow {
        id: scheduleTimeRow
        caption: "Download time"
        timestamp: Date.now() / 1000 + packet.relativeTime
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Download offset"
            anchors.verticalCenter: parent.verticalCenter
        }

        Switch {
            id: startOffsetSwitch
            checked: packet.offset === 0
        }

        Label {
            id: startOffsetText
            text: "Download file from start"
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        x: startOffsetText.x
        width: parent.width - x
        visible: !startOffsetSwitch.checked
        spacing: parent.spacing

        Label {
            text: "Offset in file:"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: offsetBox
            anchors.verticalCenter: parent.verticalCenter
            from: 0
            to: 16777215 // 2^24 - 1
            stepSize: 217 // FileDownloadData::maxLengthPerPacket
            value: packet.offset
            editable: true
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Download size"
            anchors.verticalCenter: parent.verticalCenter
        }

        Switch {
            id: wholeFileSwitch
            checked: packet.length === 0
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            id: wholeFileText
            text: "Download whole file"
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        x: wholeFileText.x
        width: parent.width - x
        visible: !wholeFileSwitch.checked

        Label {
            text: "Limit download size:"
            anchors.verticalCenter: parent.verticalCenter
        }

        // TODO: use max file size of the satellite file system?
        SpinBox {
            id: sizeBox
            anchors.verticalCenter: parent.verticalCenter
            from: 0
            to: 16777215 // 2^24 - 1
            stepSize: 1024
            value: packet.length
            editable: true
        }
    }

    Row {
        spacing: parent.spacing
        width: parent.width

        Label {
            width: leftColumnWidth
            text: "Data rate"
            height: Math.max(dataRateCombo.height, implicitHeight)
            verticalAlignment: Text.AlignVCenter
        }

        ComboBox {
            id: dataRateCombo
            textRole: "text"
            width: 200
            currentIndex: Number(packet.txDataRate)
            model: ListModel {
                id: dataRateModel

                ListElement {
                    text: "500 bps"
                    value: OBC.ComTxDatarate_500bps
                }
                ListElement {
                    text: "1250 bps"
                    value: OBC.ComTxDatarate_1250bps
                }
                ListElement {
                    text: "2500 bps"
                    value: OBC.ComTxDatarate_2500bps
                }
                ListElement {
                    text: "5000 bps"
                    value: OBC.ComTxDatarate_5000bps
                }
                ListElement {
                    text: "12500 bps"
                    value: OBC.ComTxDatarate_12500bps
                }
                ListElement {
                    text: "25000 bps"
                    value: OBC.ComTxDatarate_25000bps
                }
                ListElement {
                    text: "50000 bps"
                    value: OBC.ComTxDatarate_50000bps
                }
                ListElement {
                    text: "125000 bps"
                    value: OBC.ComTxDatarate_125000bps
                }
            }
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Delete file?"
            anchors.verticalCenter: parent.verticalCenter
        }

        Switch {
            id: deleteSwitch
            checked: packet.shouldDelete
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            text: "Delete file after download"
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
