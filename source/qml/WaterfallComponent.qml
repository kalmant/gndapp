
import QtQuick 2.9
import QtQuick.Controls 2.2
import hu.timur 1.0

Column {
    property alias sensitivityScaleValue: sensitivityScale.value
    property alias adaptiveColoringChecked: adaptiveColoring.checked
    property alias sample1024Checked: sample1024.checked
    property alias sample2048Checked: sample2048.checked
    property alias sample4096Checked: sample4096.checked
    property alias sample8192Checked: sample8192.checked
    property alias samplesToWait: waterfallPlot.samplesToWait
    property alias audioDeviceModel: audioInputDeviceCombo.model
    property alias audioDeviceCurrentIndex: audioInputDeviceCombo.currentIndex

    anchors.horizontalCenter: parent.horizontalCenter

    function startDemodulation() {
        waterfallPlot.startPackets(audioDeviceCurrentIndex);
    }

    function stopDemodulation() {
        waterfallPlot.stopPackets();
    }

    function startPlot() {
        waterfallEnabledSwitch.checked = true;
    }

    function stopPlot() {
        waterfallEnabledSwitch.checked = false;
    }

    Row {
        id: controls
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        ComboBox {
            id: audioInputDeviceCombo
            width: 275
            model: deviceDiscovery.audioDevices
            textRole: "display"
            enabled: audioInputDeviceCombo.count > 0 && !waterfallPlot.isStarted
        }
        Switch {
            id: waterfallEnabledSwitch
            text: qsTr("Plot")
            enabled: audioInputDeviceCombo.count > 0
            onCheckedChanged: {
                if (checked){
                    waterfallPlot.startWaterfall(audioDeviceCurrentIndex);
                    waterfallPlot.hasNeverRun = false;
                } else {
                    waterfallPlot.stopWaterfall();
                }
            }
        }
        Switch {
            text: qsTr("Adaptive coloring")
            id: adaptiveColoring
            checked: true
            enabled: audioInputDeviceCombo.count > 0
            onCheckedChanged: {
                if (waterfallPlot.isWaterfallActive) {
                    stopPlot();
                    waterfallPlot.setAdaptiveColoring(adaptiveColoring.checked);
                    startPlot();
                } else {
                    waterfallPlot.setAdaptiveColoring(adaptiveColoring.checked);
                }
            }
        }
        Row {
            enabled: !adaptiveColoring.checked && audioInputDeviceCombo.count > 0
            spacing: 10

            Label {
                text: qsTr("Scaling factor")
                anchors.verticalCenter: parent.verticalCenter
            }
            SpinBox {
                id: sensitivityScale
                from: 0
                to: 99999
                stepSize: 10
                value: 100
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
    Row {
        id: waterfallRow
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Waterfall {
            id: waterfallPlot

            property bool hasNeverRun: true

            objectName: "waterfallObject"
            height: 150
            width: 504
            sensitivity: sensitivityScale.value / 100
            anchors.verticalCenter: parent.verticalCenter

            // Vertical lines on the waterfall diagram for every 100 Hz
            Item {
                x: 0
                y: 0
                visible: (waterfallPlot.isWaterfallActive || !waterfallPlot.hasNeverRun)? true : false

                Row {
                    spacing: 0

                    Repeater {
                        model: 14
                        delegate: Item {
                            width: 37.111 // 37.111 pixels = 200 Hz
                            height: waterfallPlot.height

                            Text {
                                id: freqText
                                font.pixelSize: 9
                                text: String(index * 200 + 300) + " Hz"
                                style: Text.Outline
                                styleColor: "#fff"
                                font.weight: Font.DemiBold

                                transform: [
                                    Rotation{
                                        angle: 90
                                    },
                                    Translate{
                                        x:  freqText.height + 3
                                        y:  10
                                    }
                                ]
                                anchors.top: parent.top
                                anchors.left: parent.left
                            }
                            Rectangle {
                                color: "#b0ffffff"
                                width: 1
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                anchors.leftMargin: 1
                            }
                            Rectangle {
                                color: "#d0000000"
                                width: 1
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            width: 20
            height: 150
            visible: adaptiveColoring.checked
            anchors.verticalCenter: parent.verticalCenter
            gradient: Gradient {
                GradientStop { position: 0.0 ; color: "#FF0000" }
                GradientStop { position: 1.0 / 7; color: "#FF8000" }
                GradientStop { position: 2.0 / 7; color: "#FFFF00"}
                GradientStop { position: 3.0 / 7; color: "#00FF00"}
                GradientStop { position: 4.0 / 7; color: "#00FF80"}
                GradientStop { position: 5.0 / 7; color: "#00FFFF"}
                GradientStop { position: 6.0 / 7; color: "#0080FF"}
                GradientStop { position: 1.0; color: "#0000FF"}
            }
        }

        Column {
            Label {
                visible: adaptiveColoring.checked
                text: "Min: " + waterfallPlot.maxValue.toFixed(1) + " dB"
            }

            GroupBox {
                title: qsTr("Sample count")
                enabled: audioInputDeviceCombo.count > 0
                padding: 0

                Column {
                    spacing: 0

                    RadioButton {
                        id: sample1024
                        text: "1024"
                        onClicked: waterfallPlot.samplesToWait = 1024
                    }
                    RadioButton {
                        id: sample2048
                        text: "2048"
                        onClicked: waterfallPlot.samplesToWait = 2048
                    }
                    RadioButton {
                        id: sample4096
                        text: "4096"
                        checked: true
                        onClicked: waterfallPlot.samplesToWait = 4096
                    }
                    RadioButton {
                        id: sample8192
                        text: "8192"
                        onClicked: waterfallPlot.samplesToWait = 8192
                    }
                }
            }

            Label {
                visible: adaptiveColoring.checked
                text: "Max: " + waterfallPlot.minValue.toFixed(1) + " dB"
            }
        }
    }
}
