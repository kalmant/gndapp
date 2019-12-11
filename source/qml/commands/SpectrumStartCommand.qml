
import QtQuick 2.9
import QtQuick.Controls 2.2

Column {
    id: view

    property var packet
    property alias when: options.when
    property alias repeat: options.repeat

    spacing: 10
    width: 640

    function updatePacket() {
        measurementIdBox.focus = false
        startFreqBox.focus = false
        stopFreqBox.focus = false
        rbwBox.focus = false
        stepSizeBox.focus = false

        packet.measurementIdentifier = measurementIdBox.value
        packet.startTimestamp = sendNowSwitch.checked ? 0 : (scheduleTimeRow.when || (Date.now() / 1000))
        packet.startFrequency = startFreqBox.value
        packet.stopFrequency = stopFreqBox.value
        packet.rbw = rbwBox.value
        packet.stepSize = stepSizeBox.value
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Spectrum analyzer start command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "Tells the satellite to execute a spectrum measurement at the given time with the given parameters."
    }

    GenericOptions {
        id: options
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Meas. ID"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: measurementIdBox
            from: 0
            to: 65535
            value: packet.measurementIdentifier
            editable: true
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Send now"
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }

        Row {
            width: parent.width - leftColumnWidth - parent.spacing
            spacing: parent.spacing

            Switch {
                id: sendNowSwitch
                checked: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    CommandTimeRow {
        id: scheduleTimeRow
        caption: "Meas. time"
        timestamp: packet.startTimestamp
        visible: !sendNowSwitch.checked
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Preset"
            anchors.verticalCenter: parent.verticalCenter
        }

        ComboBox {
            id: presetBox

            property bool isCustom: currentIndex === 0

            anchors.verticalCenter: parent.verticalCenter
            currentIndex: {
                for (var i = 1; i < presetsModel.count; i++) {
                    var preset = presetsModel.get(i)
                    var startMatch = (preset.start === packet.startFrequency)
                    var stopMatch = (preset.stop === packet.stopFrequency)
                    var rbwMatch = (preset.rbw === packet.rbw)

                    if (startMatch && stopMatch && rbwMatch) {
                        console.warn("current index is", i, packet.startFrequency)
                        return i;
                    }
                }

                var custom = presetsModel.get(0)
                custom.start = packet.startFrequency
                custom.stop = packet.stopFrequency
                custom.rbw = packet.rbw
                return 0
            }
            textRole: "text"
            model: ListModel {
                id: presetsModel

                ListElement {
                    text: "Custom"
                    start: 460000000
                    stop: 880000000
                    rbw: 5
                }
                ListElement {
                    text: "DVB-T"
                    start: 460000000
                    stop: 860000000
                    rbw: 8
                }
                ListElement {
                    text: "GSM-900"
                    start: 824000000
                    stop: 960000000
                    rbw: 6
                }
                ListElement {
                    text: "ISM-100"
                    start: 144000000
                    stop: 146000000
                    rbw: 2
                }
                ListElement {
                    text: "ISM-400"
                    start: 432000000
                    stop: 438000000
                    rbw: 2
                }
                ListElement {
                    text: "Air"
                    start: 119000000
                    stop: 137000000
                    rbw: 5
                }
            }
            onCurrentIndexChanged: {
                if (currentIndex === 0)
                    return

                isCustom = true
                console.warn("whoah", currentIndex)

                var preset = presetsModel.get(currentIndex);
                startFreqBox.value = preset.start
                stopFreqBox.value = preset.stop
                rbwBox.value = preset.rbw

                isCustom = Qt.binding(function() { return currentIndex === 0 })
            }
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Start freq. [Hz]"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: startFreqBox
            editable: true
            anchors.verticalCenter: parent.verticalCenter
            from: 119000000
            to: 959999999
            value: {
                console.warn(presetBox.currentIndex, presetsModel.get(presetBox.currentIndex).start, packet.startFrequency)

                return presetBox.currentIndex ? presetsModel.get(presetBox.currentIndex).start : packet.startFrequency
            }

            onValueChanged: {
                if (!presetBox.isCustom && presetsModel.get(presetBox.currentIndex).start !== value)
                    presetBox.currentIndex = 0
            }
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Stop freq. [Hz]"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: stopFreqBox
            editable: true
            anchors.verticalCenter: parent.verticalCenter
            from: startFreqBox.value + 1
            to: 960000000
            value: presetsModel.get(presetBox.currentIndex).stop
            onValueChanged: {
                if (!presetBox.isCustom && presetsModel.get(presetBox.currentIndex).stop !== value)
                    presetBox.currentIndex = 0
            }
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "RBW"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: rbwBox
            editable: true
            anchors.verticalCenter: parent.verticalCenter
            from: 0
            to: 9
            value: presetsModel.get(presetBox.currentIndex).rbw
            onValueChanged: {
                if (!presetBox.isCustom && presetsModel.get(presetBox.currentIndex).rbw !== value)
                    presetBox.currentIndex = 0
            }
        }
    }

    Row {
        width: parent.width
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Step size"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: stepSizeBox
            editable: true
            anchors.verticalCenter: parent.verticalCenter
            enabled: false
            from: 0
            to: 100e6
            value: Math.pow(2, rbwBox.value) * 1500 / 4
        }
    }
}
