import QtQuick 2.9
import QtQuick.Controls 2.2
import hu.tt 1.0

Column {
    anchors.horizontalCenter: parent.horizontalCenter

    function changeToRadio() {
        spectogram.switchToRadioMode()
    }

    function changeToSDR() {
        spectogram.switchToSDRMode()
    }

    function start() {
        spectogram.startSpectogram()
    }

    function stop() {
        spectogram.stopSpectogram()
    }


    Row{
        id: spectogramRow
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Spectogram {
            id: spectogram


            objectName: "spectogramObject"
            height: spectogram.height
            width: spectogram.width
            anchors.verticalCenter: parent.verticalCenter

            // Vertical lines on the spectogram for every 100 Hz
            Item {
                x: 0
                y: 0

                Row {
                    spacing: 0

                    Repeater {
                        model: 14
                        delegate: Item {
                            width: spectogram.width / 14
                            height: spectogram.height

                            Rectangle{
                                color: "white"
                                height: freqText.implicitHeight + 2
                                width: freqText.implicitWidth + 4
                                Text {
                                    id: freqText
                                    font.pixelSize: 9
                                    text: String(Math.floor(index/14 * (spectogram.maximumFrequency - spectogram.minimumFrequency)) + Number(spectogram.minimumFrequency)) + " Hz"
                                    style: Text.Outline
                                    styleColor: "#fff"
                                    font.weight: Font.DemiBold
                                    anchors.centerIn: parent
                                }
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
    }
}
