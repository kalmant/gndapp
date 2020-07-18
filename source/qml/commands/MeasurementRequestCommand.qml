
import QtQuick 2.9
import QtQuick.Controls 2.2

Column {
    id: view

    property var packet
    property alias when: options.when
    property alias repeat: options.repeat

    spacing: 10

    function updatePacket() {
        measurementIdBox.focus = false
        durationBox.focus = false
        solarIntervalBox.focus = false
        pcuIntervalBox.focus = false
        mpuIntervalBox.focus = false
        obcComIntervalBox.focus = false

        packet.measurementIdentifier = measurementIdBox.value
        packet.startTimestamp = scheduleTimeRow.when
        packet.durationMinutes = durationBox.value

        packet.intervalSolarSec = solarIntervalBox.value
        packet.intervalPcuSec = pcuIntervalBox.value
        packet.intervalMpuDecisec = mpuIntervalBox.value / 100
        packet.intervalObcComSec = obcComIntervalBox.value

        var solar = packet.solar;
        var pcu = packet.pcu;
        var mpu = packet.mpu;
        var obcCom = packet.obcCom;

        solar.selectTemperature = solarTemperatureBox.checked ? 1 : 0
        solar.selectLightSensor = solarLightSensorBox.checked ? 1 : 0
        solar.selectInputCurrent = solarInputCurrentBox.checked ? 1 : 0
        solar.selectInputVoltage = solarInputVoltageBox.checked ? 1 : 0
        solar.selectOutputCurrent = solarOutputCurrentBox.checked ? 1 : 0
        solar.selectOutputVoltage = solarOutputVoltageBox.checked ? 1 : 0
        solar.selectTimestamp = solarTimestampBox.checked ? 1 : 0

        pcu.selectSdc1InputCurrent = sdc1InCurrBox.checked ? 1 : 0
        pcu.selectSdc1OutputCurrent = sdc1OutCurrBox.checked ? 1 : 0
        pcu.selectSdc1OutputVoltage = sdc1OutVoltBox.checked ? 1 : 0
        pcu.selectSdc2InputCurrent = sdc2InCurrBox.checked ? 1 : 0
        pcu.selectSdc2OutputCurrent = sdc2OutCurrBox.checked ? 1 : 0
        pcu.selectSdc2OutputVoltage = sdc2OutVoltBox.checked ? 1 : 0
        pcu.selectRegulatedBusVoltage = regBusVoltBox.checked ? 1 : 0
        pcu.selectUnregulatedBusVoltage = unregBusVoltBox.checked ? 1 : 0
        pcu.selectObc1Current = obc1CurrentBox.checked ? 1 : 0
        pcu.selectObc2Current = obc2CurrentBox.checked ? 1 : 0
        pcu.selectBatteryVoltage = batteryVoltageBox.checked ? 1 : 0
        pcu.selectBatteryChargeCurrent = batteryChargeCurrentBox.checked ? 1 : 0
        pcu.selectBatteryDischargeCurrent = batteryDischargeCurrentBox.checked ? 1 : 0
        pcu.selectBatteryChargeEnabled = batteryChargeEnabledBox.checked ? 1 : 0
        pcu.selectBatteryDischargeEnabled = batteryDischargeEnabledBox.checked ? 1 : 0
        pcu.usePcu2 = usePcu2Switch.checked ? 1 : 0

        mpu.selectMpu1Temperature = mpu1TemperatureBox.checked ? 1 : 0
        mpu.selectMpu1Gyroscope = mpu1GyroBox.checked ? 1 : 0
        mpu.selectMpu1Magnetometer = mpu1MagnetoBox.checked ? 1 : 0
        mpu.selectMpu2Temperature = mpu2TemperatureBox.checked ? 1 : 0
        mpu.selectMpu2Gyroscope = mpu2GyroBox.checked ? 1 : 0
        mpu.selectMpu2Magnetometer = mpu2MagnetoBox.checked ? 1 : 0

        obcCom.selectRtcc1Temperature = rtcc1TemperatureBox.checked ? 1 : 0
        obcCom.selectRtcc2Temperature = rtcc2TemperatureBox.checked ? 1 : 0
        obcCom.selectObcTemperature = obcTemperatureBox.checked ? 1 : 0
        obcCom.selectComCurrent = comCurrentBox.checked ? 1 : 0
        obcCom.selectEps2aTemperature1 = eps2aTemp1Box.checked ? 1 : 0
        obcCom.selectEps2aTemperature2 = eps2aTemp2Box.checked ? 1 : 0
        obcCom.selectComVoltage = comVoltageBox.checked ? 1 : 0
        obcCom.selectComTemperature = comTemperatureBox.checked ? 1 : 0
        obcCom.selectSpectrumTemperature = spectrumTemperatureBox.checked ? 1 : 0
        obcCom.silentMode = silentModeBox.checked ? 1 : 0

        packet.solar = solar;
        packet.pcu = pcu;
        packet.mpu = mpu;
        packet.obcCom = obcCom;
    }

    Label {
        font.bold: true
        width: parent.width
        text: "Measurement request command"
    }

    Label {
        width: parent.width
        wrapMode: Text.Wrap
        text: "Tells the satellite to execute a universal measurement at the given time with the given parameters."
    }

    GenericOptions {
        id: options
    }

    Row {
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Meas. ID"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: measurementIdBox
            from: 1
            to: 65535
            value: packet.measurementIdentifier
            editable: true
            Component.onCompleted: contentItem.selectByMouse = true
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    CommandTimeRow {
        id: scheduleTimeRow
        caption: "Meas. time"
        timestamp: packet.startTimestamp
    }

    Row {
        spacing: parent.spacing

        Label {
            width: leftColumnWidth
            text: "Duration"
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinBox {
            id: durationBox
            from: 1
            to: 65535
            value: packet.durationMinutes
            editable: true
            Component.onCompleted: contentItem.selectByMouse = true
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            text: "minutes"
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Column {
        TabBar {
            id: measurementSelectionTabBar

            TabButton {
                text: "Solar"
                width: implicitWidth + 20
            }

            TabButton {
                text: "PCU"
                width: implicitWidth + 20
            }

            TabButton {
                text: "MPU"
                width: implicitWidth + 20
            }

            TabButton {
                text: "OBC / COM"
                width: implicitWidth + 20
            }
        }

        Frame {
            height: tabsRow.height + 2 * padding
            width: tabsRow.width + 2 * padding
            background: Rectangle {
                width: parent.width
                height: parent.height + 1
                y: -1
                color: palette.base
                border.color: palette.mid
            }

            Row {
                id: tabsRow
                spacing: 10
                width: Math.max(solarMeasurementColumn.width, pcuMeasurementColumn.width, mpuMeasurementColumn.width, obcComMeasurementColumn.width)

                Column {
                    id: solarMeasurementColumn
                    spacing: parent.spacing
                    visible: measurementSelectionTabBar.currentIndex === 0

                    Row {
                        spacing: parent.spacing

                        Switch {
                            id: enableSolarSwitch
                            text: "Collect Solar data"
                            checked: packet.intervalSolarSec > 0
                            onCheckedChanged: solarIntervalBox.value = solarIntervalBox.from = checked ? packet.intervalSolarSec : 0
                        }

                        Label {
                            enabled: enableSolarSwitch.checked
                            text: "Interval in seconds"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        SpinBox {
                            id: solarIntervalBox
                            enabled: enableSolarSwitch.checked
                            from: enableSolarSwitch.checked ? 1 : 0
                            to: 5 * 60
                            stepSize: 5
                            value: packet.intervalSolarSec
                            onValueChanged: packet.intervalSolarSec = value
                            editable: true
                            Component.onCompleted: contentItem.selectByMouse = true
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Grid {
                        columns: 3
                        enabled: enableSolarSwitch.checked

                        CheckBox {
                            id: solarTemperatureBox
                            text: "Temperature"
                            checked: !!packet.solar.selectTemperature
                        }

                        CheckBox {
                            id: solarLightSensorBox
                            text: "Light sensor"
                            checked: !!packet.solar.selectLightSensor
                        }

                        CheckBox {
                            id: solarInputCurrentBox
                            text: "Input current"
                            checked: !!packet.solar.selectInputCurrent
                        }

                        CheckBox {
                            id: solarInputVoltageBox
                            text: "Input voltage"
                            checked: !!packet.solar.selectInputVoltage
                        }

                        CheckBox {
                            id: solarOutputCurrentBox
                            text: "Output current"
                            checked: !!packet.solar.selectOutputCurrent
                        }

                        CheckBox {
                            id: solarOutputVoltageBox
                            text: "Output voltage"
                            checked: !!packet.solar.selectOutputVoltage
                        }

                        CheckBox {
                            id: solarTimestampBox
                            text: "Timestamp"
                            checked: !!packet.solar.selectTimestamp
                        }
                    }
                }


                Column {
                    id: pcuMeasurementColumn
                    spacing: parent.spacing
                    visible: measurementSelectionTabBar.currentIndex === 1

                    Row {
                        spacing: parent.spacing

                        Switch {
                            id: enablePcuSwitch
                            text: "Collect PCU data"
                            checked: packet.intervalPcuSec > 0
                            onCheckedChanged: pcuIntervalBox.value = pcuIntervalBox.from = checked ? packet.intervalPcuSec : 0
                        }

                        Switch {
                            id: usePcu2Switch
                            enabled: enablePcuSwitch.checked
                            text: "Use PCU2"
                            checked: !!packet.pcu.usePcu2
                        }

                        Label {
                            text: "Interval in seconds"
                            enabled: enablePcuSwitch.checked
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        SpinBox {
                            id: pcuIntervalBox
                            enabled: enablePcuSwitch.checked
                            from: enablePcuSwitch.checked ? 1 : 0
                            to: 5 * 60
                            stepSize: 5
                            value: packet.intervalPcuSec
                            onValueChanged: packet.intervalPcuSec = value
                            editable: true
                            Component.onCompleted: contentItem.selectByMouse = true
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Grid {
                        columns: 3
                        enabled: enablePcuSwitch.checked

                        CheckBox {
                            id: sdc1InCurrBox
                            text: "SDC1 input current"
                            checked: !!packet.pcu.selectSdc1InputCurrent
                        }

                        CheckBox {
                            id: sdc1OutCurrBox
                            text: "SDC1 output current"
                            checked: !!packet.pcu.selectSdc1OutputCurrent
                        }

                        CheckBox {
                            id: sdc1OutVoltBox
                            text: "SDC1 output voltage"
                            checked: !!packet.pcu.selectSdc1OutputVoltage
                        }

                        CheckBox {
                            id: sdc2InCurrBox
                            text: "SDC2 input current"
                            checked: !!packet.pcu.selectSdc2InputCurrent
                        }

                        CheckBox {
                            id: sdc2OutCurrBox
                            text: "SDC2 output current"
                            checked: !!packet.pcu.selectSdc2OutputCurrent
                        }

                        CheckBox {
                            id: sdc2OutVoltBox
                            text: "SDC2 output voltage"
                            checked: !!packet.pcu.selectSdc2OutputVoltage
                        }

                        CheckBox {
                            id: regBusVoltBox
                            text: "Regulated bus voltage"
                            checked: !!packet.pcu.selectRegulatedBusVoltage
                        }

                        CheckBox {
                            id: unregBusVoltBox
                            text: "Unregulated bus voltage"
                            checked: !!packet.pcu.selectUnregulatedBusVoltage
                        }

                        CheckBox {
                            id: obc1CurrentBox
                            text: "OBC1 current"
                            checked: !!packet.pcu.selectObc1Current
                        }

                        CheckBox {
                            id: obc2CurrentBox
                            text: "OBC2 current"
                            checked: !!packet.pcu.selectObc2Current
                        }

                        CheckBox {
                            id: batteryVoltageBox
                            text: "Battery voltage"
                            checked: !!packet.pcu.selectBatteryVoltage
                        }

                        CheckBox {
                            id: batteryChargeCurrentBox
                            text: "Battery charge current"
                            checked: !!packet.pcu.selectBatteryChargeCurrent
                        }

                        CheckBox {
                            id: batteryDischargeCurrentBox
                            text: "Battery discharge current"
                            checked: !!packet.pcu.selectBatteryDischargeCurrent
                        }

                        CheckBox {
                            id: batteryChargeEnabledBox
                            text: "Battery charge enabled"
                            checked: !!packet.pcu.selectBatteryChargeEnabled
                        }

                        CheckBox {
                            id: batteryDischargeEnabledBox
                            text: "Battery discharge enabled"
                            checked: !!packet.pcu.selectBatteryDischargeEnabled
                        }

                    }
                }

                Column {
                    id: mpuMeasurementColumn
                    spacing: parent.spacing
                    visible: measurementSelectionTabBar.currentIndex === 2

                    Row {
                        spacing: parent.spacing

                        Switch {
                            id: enableMpuSwitch
                            text: "Collect MPU data"
                            checked: packet.intervalMpuDecisec > 0
                            onCheckedChanged: mpuIntervalBox.value = packet.intervalMpuDecisec * (mpuIntervalBox.from = checked ? 100 : 0)
                        }

                        Label {
                            text: "Interval in milliseconds"
                            enabled: enableMpuSwitch.checked
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        SpinBox {
                            id: mpuIntervalBox
                            enabled: enableMpuSwitch.checked
                            from: enableMpuSwitch.checked ? 1000 : 0
                            stepSize: 100
                            to: 5 * 60 * 1000
                            value: packet.intervalMpuDecisec * 100
                            onValueChanged: packet.intervalMpuDecisec = value / 100
                            editable: true
                            Component.onCompleted: contentItem.selectByMouse = true
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Grid {
                        columns: 3
                        enabled: enableMpuSwitch.checked

                        CheckBox {
                            id: mpu1TemperatureBox
                            text: "MPU1 temperature"
                            checked: !!packet.mpu.selectMpu1Temperature
                        }

                        CheckBox {
                            id: mpu1GyroBox
                            text: "MPU1 gyroscope"
                            checked: !!packet.mpu.selectMpu1Gyroscope
                        }

                        CheckBox {
                            id: mpu1MagnetoBox
                            text: "MPU1 magnetometer"
                            checked: !!packet.mpu.selectMpu1Magnetometer
                        }

                        CheckBox {
                            id: mpu2TemperatureBox
                            text: "MPU2 temperature"
                            checked: !!packet.mpu.selectMpu2Temperature
                        }

                        CheckBox {
                            id: mpu2GyroBox
                            text: "MPU2 gyroscope"
                            checked: !!packet.mpu.selectMpu2Gyroscope
                        }

                        CheckBox {
                            id: mpu2MagnetoBox
                            text: "MPU2 magnetometer"
                            checked: !!packet.mpu.selectMpu2Magnetometer
                        }
                    }

                }

                Column {
                    id: obcComMeasurementColumn
                    spacing: parent.spacing
                    visible: measurementSelectionTabBar.currentIndex === 3

                    Row {
                        spacing: parent.spacing

                        Switch {
                            id: enableObcComSwitch
                            text: "Collect OBC/COM data"
                            checked: packet.intervalObcComSec > 0
                            onCheckedChanged: obcComIntervalBox.value = obcComIntervalBox.from = checked ? packet.intervalObcComSec : 0
                        }

                        Label {
                            text: "Interval in seconds"
                            enabled: enableObcComSwitch.checked
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        SpinBox {
                            id: obcComIntervalBox
                            enabled: enableObcComSwitch.checked
                            from: enableObcComSwitch.checked ? 1 : 0
                            to: 5 * 60
                            value: packet.intervalObcComSec
                            onValueChanged: packet.intervalObcComSec = value
                            editable: true
                            Component.onCompleted: contentItem.selectByMouse = true
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Grid {
                        columns: 3
                        enabled: enableObcComSwitch.checked

                        CheckBox {
                            id: rtcc1TemperatureBox
                            text: "RTCC1 temperature"
                            checked: packet.obcCom.selectRtcc1Temperature
                        }

                        CheckBox {
                            id: rtcc2TemperatureBox
                            text: "RTCC2 temperature"
                            checked: packet.obcCom.selectRtcc2Temperature
                        }

                        CheckBox {
                            id: obcTemperatureBox
                            text: "OBC temperature"
                            checked: packet.obcCom.selectObcTemperature
                        }

                        CheckBox {
                            id: comCurrentBox
                            text: "COM current"
                            checked: packet.obcCom.selectComCurrent
                        }

                        CheckBox {
                            id: eps2aTemp1Box
                            text: "EPS2A temperature#1"
                            checked: packet.obcCom.selectEps2aTemperature1
                        }

                        CheckBox {
                            id: eps2aTemp2Box
                            text: "EPS2A temperature#2"
                            checked: packet.obcCom.selectEps2aTemperature2
                        }

                        CheckBox {
                            id: comVoltageBox
                            text: "COM voltage"
                            checked: packet.obcCom.selectComVoltage
                        }

                        CheckBox {
                            id: comTemperatureBox
                            text: "COM temperature"
                            checked: packet.obcCom.selectComTemperature
                        }

                        CheckBox {
                            id: spectrumTemperatureBox
                            text: "Spectrum temperature"
                            checked: packet.obcCom.selectSpectrumTemperature
                        }

                        CheckBox {
                            id: silentModeBox
                            text: "Silent mode"
                            checked: packet.obcCom.silentMode
                        }

                    }
                }

            }
        }
    }
}
