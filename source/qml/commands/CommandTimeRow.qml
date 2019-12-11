
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2 as Dialogs1
import ".."

Row {
    id: commandTimeRow

    readonly property var when: asapSwitch.checked ? 0 : dateTimeButton.timestamp
    property alias caption: captionText.text
    property alias timestamp: dateTimeButton.timestamp

    spacing: 10
    width: parent.width

    Text {
        id: captionText
        width: leftColumnWidth
        text: "Send when?"
        height: Math.max(asapSwitch.height, implicitHeight)
        verticalAlignment: Text.AlignVCenter
    }

    Column {
        width: parent.width - 100 - parent.spacing
        spacing: 10

        Row {
            width: parent.width
            spacing: 10

            Switch {
                id: asapSwitch
                checked: timestamp === 0
                onCheckedChanged: {
                    if (!checked && !timestamp) {
                        timestamp = Math.floor(new Date().valueOf() / 1000)
                    }
                }
            }

            Label {
                id: rightNowLabel
                text: "Right now"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Row {
            x: rightNowLabel.x
            width: parent.width - x
            spacing: 10
            visible: !asapSwitch.checked

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: (new Date(dateTimeButton.timestamp * 1000)).toUTCString()
            }

            Button {
                id: dateTimeButton

                property var timestamp: 0

                text: "Change"
                onClicked: {
                    var obj = dateTimePickerDialogComponent.createObject(commandDialog)
                    obj.open()
                }

                Component {
                    id: dateTimePickerDialogComponent

                    Dialogs1.Dialog {
                        modality: Qt.ApplicationModal
                        width: dateTimePicker.width
                        height: dateTimePicker.height

                        onVisibleChanged: {
                            if (visible) {
                                dateTimePicker.reset()
                            }
                        }

                        onAccepted: {
                            dateTimeButton.timestamp = Math.floor(dateTimePicker.getDateTime() / 1000)
                            destroy()
                        }

                        onRejected: {
                            destroy()
                        }

                        DateTimePicker {
                            id: dateTimePicker
                        }
                    }
                }
            }
        }
    }
}
