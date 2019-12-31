
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2 as Dialogs1

ScrollView {
    id: scrollView
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    anchors.fill: parent
    clip: true
    padding: 10

    property bool compactLayout: mainWindow.width < 1200
    property int saveSettingsOnExit: 2
    property var bottomSectionSwitch: waterfallDiagramSwitch
    property var baseFreq: baseFrequency

    // TODO: figure out a better way to handle audioInWaterfall, since its defined in main.qml

    //Saves every setting to the SettingsHolder object and calls saveSettings() on settingsProxy
    function saveSettings(){
        var WDNIndex = audioInWaterfall.audioDeviceCurrentIndex;
        var WDName = audioInWaterfall.audioDeviceModel.data(audioInWaterfall.audioDeviceModel.index(WDNIndex,0));
        if (typeof WDName != 'undefined'){
            settingsHolder.wdn = WDName;
        } else {
            settingsHolder.wdn = "unspecified";
        }
        settingsHolder.wac = audioInWaterfall.adaptiveColoringChecked;
        settingsHolder.wsf = audioInWaterfall.sensitivityScaleValue;
        settingsHolder.wsc = audioInWaterfall.samplesToWait;

        settingsHolder.sswos = waterfallDiagramSwitch.checked;

        settingsHolder.tsi = parseInt(satIdInput.text);
        if (!latitudeInput.acceptableInput){
            latitudeInput.text = "0"
        }
        if (!longitudeInput.acceptableInput){
            longitudeInput.text = "0"
        }
        if (!elevationInput.acceptableInput){
            elevationInput.text = "0"
        }
        if (latitudeCombo.currentIndex === 0){
            settingsHolder.tsla = latitudeInput.text;
        } else {
            settingsHolder.tsla = "-"+latitudeInput.text;
        }
        if (longitudeCombo.currentIndex === 0) {
            settingsHolder.tslo = longitudeInput.text;
        } else {
            settingsHolder.tslo = "-"+longitudeInput.text;
        }
        settingsHolder.tse = parseInt(elevationInput.text);

        settingsHolder.sdroffs = sdrOffsetSpinbox.value;
        settingsHolder.sdrppm = sdrPPMSpinbox.value;
        settingsHolder.sdrgain = sdrGainSpinbox.value;
        settingsHolder.sdradft = sdrDFTrackingSwitch.checked;
        settingsHolder.sdrdf = sdrDFSpinbox.value;

        var raCOMName = radioCOMCombo.model[radioCOMCombo.currentIndex];
        if (typeof raCOMName != 'undefined'){
            settingsHolder.racp = raCOMName;
        } else {
            settingsHolder.racp = "unspecified";
        }
        settingsHolder.rabr = radioBaudRate.model[radioBaudRate.currentIndex];
        settingsHolder.raoffs = radioOffsetSpinbox.value;
        settingsHolder.ram = radioModelCombo.model[radioModelCombo.currentIndex];
        settingsHolder.rasrto = ft817OnOffSwitch.checked;
        settingsHolder.rasr5voso = smogRadio5VOutSwitch.checked;

        var roCOMName = rotatorCOMCombo.model[rotatorCOMCombo.currentIndex];
        if (typeof roCOMName != 'undefined'){
            settingsHolder.rocp = roCOMName;
        } else {
            settingsHolder.rocp = "unspecified";
        }
        settingsHolder.robr = rotatorBaudRate.model[rotatorBaudRate.currentIndex];
        settingsHolder.rom = rotatorModelCombo.model[rotatorModelCombo.currentIndex];
        settingsHolder.rocde = rotatorMaxElev.currentIndex == 0 ? false : true;
        settingsHolder.roazsp = rotatorAzimuthStopSpin.value;
        settingsHolder.rosp = rotatorParkingSwitch.checked;
        settingsHolder.ropaz = parkingAZSpin.value;
        settingsHolder.ropelev = parkingELSpin.value;

        settingsHolder.mnpae = newPacketBottomRadio.checked == true ? true : false ;
        settingsHolder.mssoe = alwaysRadio.checked == true ? 0 : (neverRadio.checked == true ? 1 : 2);
        settingsHolder.msatidx = satelliteSelectorCombo.currentIndex

        settingsHolder.uu = uploadUsernameTF.text.trim();
        settingsHolder.up = uploadPasswordTF.text;
        settingsHolder.ual = autoLoginSwitch.checked;
        settingsHolder.uauf = uploaderAUFSB.value;

        settingsHandler.saveSettings();
    }

    //processing the values in the input fields and checking whether they are valid
    function stationApplyButtonClicked() {
        if (!latitudeInput.acceptableInput){
            latitudeInput.text = "0"
        }
        if (!longitudeInput.acceptableInput){
            longitudeInput.text = "0"
        }
        if (!elevationInput.acceptableInput){
            elevationInput.text = "0"
        }
        var tempLat = Number.fromLocaleString(Qt.locale(), latitudeInput.text)
        if (latitudeCombo.currentIndex !== 0) {
            tempLat = tempLat * (-1)
        }
        var tempLon = Number.fromLocaleString(Qt.locale(), longitudeInput.text)
        if (longitudeCombo.currentIndex !== 0) {
            tempLon = tempLon * (-1)
        }
        var tempElev = parseInt(elevationInput.text)

        predicterController.changeStation(tempLat,tempLon,tempElev);

    }

    Component.onCompleted: {
        settingsHandler.loadSettings();
        console.log("Base frequency is "+baseFreq+" Hz.");
    }


    //  Connections for loading settings
    //  Every signal loads settings associated with that group
    Connections {
        target: settingsHandler
        onLoadSoundcardSettings: {
            waterfallDiagramSwitch.checked = showWaterfallOnStartup;
        }
        onLoadSDRSettings:{
            //Ask if this can be removed
            if (offset>=-25000 && offset<=25000){
                sdrOffsetSpinbox.value = offset;
            } else {
                sdrOffsetSpinbox.value = 0;
            }

            if (PPM>=-225 && PPM<=225){
                sdrPPMSpinbox.value = PPM;
            } else {
                sdrPPMSpinbox.value = 0;
            }

            if (gain>=-50 && gain<=50){
                sdrGainSpinbox.value = gain;
            } else {
                sdrGainSpinbox.value = 0;
            }

            // Ask if these 2 can be removed
            if (DF>=-25000 && DF<=25000){
                sdrDFSpinbox.value = DF;
            } else {
                sdrDFSpinbox.value = 0;
            }

            sdrDFTrackingSwitch.checked = autoDFTrack;
        }
        onLoadTrackingSettings:{
            satIdInput.text = satelliteId;
            var sLaFloat = Number.fromLocaleString(Qt.locale(), stationLat);
            if (sLaFloat>=-90 && sLaFloat<=0){
                latitudeInput.text = Number(-sLaFloat).toLocaleString(Qt.locale(), "f", 5);
                latitudeCombo.currentIndex = 1;
            } else if (sLaFloat>=0 && sLaFloat<=90){
                latitudeInput.text = Number(sLaFloat).toLocaleString(Qt.locale(), "f", 5);
                latitudeCombo.currentIndex = 0;
            } else {
                latitudeInput.text = "0";
            }

            var sLoFloat = Number.fromLocaleString(Qt.locale(), stationLong);
            if (sLoFloat>=-180 && sLoFloat<=0){
                longitudeInput.text = Number(-sLoFloat).toLocaleString(Qt.locale(), "f", 5);
                longitudeCombo.currentIndex = 1;
            } else if (sLoFloat>=0 && sLoFloat<=180){
                longitudeInput.text = Number(sLoFloat).toLocaleString(Qt.locale(), "f", 5);
                longitudeCombo.currentIndex = 0;
            } else {
                longitudeInput.text = "0";
            }

            if (stationElev>=-10000 && stationElev<=10000){
                elevationInput.text = stationElev.toString();
            } else {
                elevationInput.text = "0";
            }

            stationApplyButtonClicked();
        }
        onLoadRadioSettings:{
            var cpIndex = 0;
            for (var i = 0; i < radioCOMCombo.model.rowCount(); i++){
                if (radioCOMCombo.model.data(radioCOMCombo.model.index(i,0)) === COMPort.toString()){
                    cpIndex = i;
                    break;
                }
            }
            radioCOMCombo.currentIndex = cpIndex;

            var brIndex = radioBaudRate.model.indexOf(""+baudRate);
            radioBaudRate.currentIndex = brIndex >= 0 ? brIndex: 0;

            if (offset>=-25000 && offset<=25000){
                radioOffsetSpinbox.value = offset;
            } else {
                radioOffsetSpinbox.value = 0;
            }

            var mIndex = radioModelCombo.model.indexOf(""+model);
            radioModelCombo.currentIndex = mIndex >= 0 ? mIndex: 0;

            ft817OnOffSwitch.checked = shouldRadioTurnOn;

            smogRadio5VOutSwitch.checked = smogRadio5VOutSwitchOn;
        }
        onLoadRotatorSettings:{
            var cpIndex = 0;
            for (var i = 0; i < rotatorCOMCombo.model.rowCount(); i++){
                if (rotatorCOMCombo.model.data(rotatorCOMCombo.model.index(i,0)) === COMPort.toString()){
                    cpIndex = i;
                    break;
                }
            }
            rotatorCOMCombo.currentIndex = cpIndex;

            var brIndex = rotatorBaudRate.model.indexOf(""+baudRate);
            rotatorBaudRate.currentIndex = brIndex >= 0 ? brIndex: 0;

            var mIndex = rotatorModelCombo.model.indexOf(""+model);
            rotatorModelCombo.currentIndex = mIndex >= 0 ? mIndex: 0;

            if (canDo180Elev) {
                rotatorMaxElev.currentIndex = 1;
            } else {
                rotatorMaxElev.currentIndex = 0;
            }

            if (azStopPoint>=-1 && azStopPoint<=359){
                rotatorAzimuthStopSpin.value = azStopPoint;
            } else {
                rotatorAzimuthStopSpin.value = 0;
            }

            if (parkingAz>=0 && parkingAz<=360){
                parkingAZSpin.value = parkingAz;
            } else {
                parkingAZSpin.value = 0;
            }

            if (parkingElev>=0 && parkingElev<=90){
                parkingELSpin.value = parkingElev;
            } else {
                parkingELSpin.value = 0;
            }

            rotatorParkingSwitch.checked = shouldPark;
        }
        onLoadWaterfallSettings:{
            var dnIndex = 0;
            for (var i = 0; i < audioInWaterfall.audioDeviceModel.rowCount(); i++){
                if (audioInWaterfall.audioDeviceModel.data(audioInWaterfall.audioDeviceModel.index(i,0)) === deviceName.toString()){
                    dnIndex = i;
                    break;
                }
            }
            audioInWaterfall.audioDeviceCurrentIndex = dnIndex;

            if (scalingFactor>=0 && scalingFactor<=99999){
                audioInWaterfall.sensitivityScaleValue = scalingFactor;
            } else {
                audioInWaterfall.sensitivityScaleValue = 100;
            }

            audioInWaterfall.adaptiveColoringChecked = adaptiveColoring;

            switch (sampleCount){
            case 1024:
                audioInWaterfall.sample1024Checked = true;
                audioInWaterfall.samplesToWait = 1024;
                break;
            case 2048:
                audioInWaterfall.sample2048Checked = true;
                audioInWaterfall.samplesToWait = 2048;
                break;
            case 4096:
                audioInWaterfall.sample4096Checked = true;
                audioInWaterfall.samplesToWait = 4096;
                break;
            case 8192:
                audioInWaterfall.sample8192Checked = true;
                audioInWaterfall.samplesToWait = 8192;
                break;
            default:
                audioInWaterfall.sample1024Checked = true;
                audioInWaterfall.samplesToWait = 1024;
                break;
            }
        }
        onLoadMiscSettings:{
            if (newPacketsAtEnd){
                newPacketBottomRadio.checked = true;
                packetTableModel.insertAtEnd = true;
            } else {
                newPacketTopRadio.checked = true;
                packetTableModel.insertAtEnd = false;
            }

            switch (saveSettingsOnExit){
            case 0:
                alwaysRadio.checked = true;
                saveSettingsOnExit = 0;
                break;
            case 1:
                neverRadio.checked = true;
                saveSettingsOnExit = 1;
                break;
            case 2:
                askRadio.checked = true;
                saveSettingsOnExit = 2;
                break;
            default:
                askRadio.checked = true;
                saveSettingsOnExit = 2;
                break;
            }
            if (satelliteIndex < 0 || satelliteIndex >= satelliteSelectorCombo.model.length){
                satelliteSelectorCombo.currentIndex = 0
            } else {
                satelliteSelectorCombo.currentIndex = satelliteIndex
            }

        }
        onLoadUploadSettings:{
            uploadUsernameTF.text = username;
            uploadPasswordTF.text = password;
            autoLoginSwitch.checked = automaticLogin;
            uploaderAUFSB.value = automaticUploadFrequency;
            if (automaticLogin && !uploader.isLoggedIn && !uploader.isCurrentlyLoggingIn && username.length > 0 && password.length > 0)
                uploader.login(username, password);
        }
    }

    Column {
        width: scrollView.width - 2 * scrollView.padding
        spacing: 10

        GroupBox {
            width: parent.width
            title: qsTr("Soundcard")
            id: scgb
            padding: 10

            Column {
                width: parent.width - 2 * parent.padding

                Switch {
                    id: soundcardInReceivePackets
                    text: enabled? qsTr("Demodulate soundcard"): '<font color="red">Demodulate soundcard is only allowed if an audio device is connected</font>'
                    enabled: deviceDiscovery.audioDeviceCount > 0
                    onCheckedChanged: {
                        if (checked){
                            audioInWaterfall.startDemodulation();
                        } else {
                            audioInWaterfall.stopDemodulation();
                        }
                    }
                    width: Math.max(soundcardInReceivePackets.implicitWidth, waterfallDiagramSwitch.implicitWidth)

                    Connections {
                        target: messageProxy
                        onAudioDeviceDisconnected: {
                            audioInWaterfall.stopPlot();
                            soundcardInReceivePackets.checked = false;
                            logger.writeToLog("Audio error: The device has been disconnected during operation.");
                            audioDeviceDisconnected.open();
                        }
                        onAudioDeviceFailed: {
                            audioInWaterfall.stopPlot();
                            soundcardInReceivePackets.checked = false;
                            logger.writeToLog("Audio error: The device could not be started.");
                            audioDeviceFailedError.open();
                        }
                    }
                }

                Row {
                    x: soundcardInReceivePackets.indicator.width + soundcardInReceivePackets.padding * 2
                    width: parent.width - x
                    spacing: parent.spacing

                    CheckBox {
                        id: waterfallDiagramSwitch
                        checked: true
                        text: qsTr("Show waterfall")
                        onCheckedChanged: {
                            if (waterfallDiagramSwitch.checked){
                                audioInWaterfall.visible = true;
                            } else {
                                audioInWaterfall.stopPlot();
                                audioInWaterfall.visible = false;
                            }
                        }
                    }
                }
            }
        }

        GroupBox {
            id: sdrgb
            title: qsTr("Software Defined Radio")
            width: parent.width

            Column {
                spacing: 10

                Switch {
                    id: sdrEnabledSwitch
                    text: enabled? qsTr("Demodulate Software Defined Radio"): '<font color="red">SDR demodulation is only allowed if a device is connected. Refresh the devices after connecting one.</font>'
                    onCheckedChanged: {
                        if (sdrDeviceCombo.currentIndex === -1){
                            sdrDeviceUnchosen.open();
                            sdrEnabledSwitch.checked = false;
                            return;
                        }
                        if (checked){
                            sdrThread.startReading(sdrDeviceCombo.currentIndex, sdrPPMSpinbox.value, sdrGainSpinbox.value, sdrOffsetSpinbox.value,sdrDFSpinbox.value,sdrDFTrackingSwitch.checked );
                        } else {
                            sdrThread.stopReading();
                        }
                    }
                    enabled: sdrThread.sdrDevices.rowCount() > 0
                    Connections {
                        target: sdrThread
                        onCannotConnectToSDRSignal: {
                            sdrEnabledSwitch.checked = false;
                            sdrErrorNoSDR.open();
                            logger.writeToLog("SDR error: No valid device was found. Make sure it is connected and the necessary drivers are installed!"+
                                              "For windows, you can find a detailed howto at https://www.rtl-sdr.com/rtl-sdr-quick-start-guide/");
                        }
                        onSdrDisconnected: {
                            sdrEnabledSwitch.checked = false;
                            sdrErrorSDRDisconnected.open();
                            logger.writeToLog("SDR error: The SDR was disconnected during operation.");
                        }
                        onInvalidSdrDeviceIndex: {
                            sdrEnabledSwitch.checked = false;
                            invalidSdrDeviceIndex.open();
                            logger.writeToLog("SDR error: SDR was attempted to start with an invalid index.");
                        }
                    }
                }

                Column {
                    x: sdrEnabledSwitch.indicator.width + sdrEnabledSwitch.padding * 2
                    spacing: parent.spacing

                    Grid {
                        spacing: parent.spacing
                        verticalItemAlignment: Grid.AlignVCenter
                        columns: 2

                        CheckBox {
                            text: qsTr("Doppler correction")
                            enabled: !sdrEnabledSwitch.checked
                            id: sdrDFTrackingSwitch
                            checked : false
                        }
                    }

                    Grid {
                        spacing: parent.spacing
                        verticalItemAlignment: Grid.AlignVCenter
                        columns: 3

                        Label {
                            text: qsTr("Device")
                        }
                        ComboBox {
                            id: sdrDeviceCombo
                            width: 275
                            currentIndex: sdrThread.sdrDevices.rowCount() > 0 ? (sdrThread.sdrDevices.rowCount() > 1 ? 1 : 0) : -1
                            model: sdrThread.sdrDevices
                            textRole: "display"
                            enabled: !sdrEnabledSwitch.checked
                        }
                        Button {
                            text: qsTr("Refresh devices")
                            onClicked: {
                                sdrThread.refreshSdrDevices()
                            }
                            enabled: !sdrEnabledSwitch.checked
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Refresh the list of SDR devices you have connected to your computer")

                        }
                    }


                    Grid {
                        enabled: !sdrEnabledSwitch.checked
                        spacing: parent.spacing
                        verticalItemAlignment: Grid.AlignVCenter
                        columns: compactLayout ? 4 : 8

                        Label {
                            text: qsTr("Error [PPM]")
                        }

                        SpinBox {
                            id: sdrPPMSpinbox
                            from: -225
                            to: 225
                            value: 0
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Allows you to set the PPM of your particular device.")
                        }

                        Label {
                            text: qsTr("Gain [dB]")
                        }

                        SpinBox {
                            id: sdrGainSpinbox
                            from: -50
                            to: 50
                            value: 0
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Allows you to set the gain of your particular device.")
                        }

                        Label {
                            text: qsTr("Offset [Hz]")
                        }

                        SpinBox{
                            id: sdrOffsetSpinbox
                            from: -25000
                            to: 25000
                            value: 0
                            stepSize: 5
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Adds a constant frequency offset to the SDR's frequency.")
                        }

                        Label {
                            enabled: !sdrDFTrackingSwitch.checked
                            text: qsTr("Doppler frequency [Hz]")
                        }

                        SpinBox {
                            id: sdrDFSpinbox
                            enabled: !sdrDFTrackingSwitch.checked
                            from: -25000
                            to: 25000
                            value: 0
                            stepSize: 5
                            onValueChanged: {
                                sdrThread.setDopplerFrequency(value)
                            }
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Manual doppler input that is enabled when tracking is not on.")
                        }
                    }
                }
            }
        }

        GroupBox {
            id: trackingGroupBox
            width: parent.width
            title: "Tracking"

            Column {
                spacing: 10

                Row {
                    spacing: parent.spacing

                    Switch {
                        id: trackingSwitch
                        text: qsTr("Track the satellite")
                        enabled: !predicterController.loading
                        onCheckedChanged: {
                            if (checked){
                                stationApplyButtonClicked();
                                predicterController.updateTLE();
                            } else {
                                predicterController.stopTracking()
                            }
                        }
                    }

                    BusyIndicator {
                        running: predicterController.loading
                        height: trackingSwitch.height + 5
                        width: height
                    }
                }

                Row {
                    spacing: parent.spacing
                    x: trackingSwitch.indicator.width + trackingSwitch.padding * 2

                    Grid {
                        spacing: parent.spacing
                        enabled: !trackingSwitch.checked
                        columns: compactLayout ? 3 : 6
                        verticalItemAlignment: Grid.AlignVCenter

                        Label {
                            text: qsTr("Latitude [°]")
                        }

                        TextField {
                            id: latitudeInput
                            maximumLength: 8
                            text: Number(47.48675).toLocaleString(Qt.locale(), "f", 5)
                            validator: DoubleValidator {
                                bottom: 0
                                top: 90
                                decimals: 5
                                notation: DoubleValidator.StandardNotation
                            }
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Your latitude or longitude is never sent to our server through this application!")
                        }

                        ComboBox {
                            id: latitudeCombo
                            model: ['North','South']
                            currentIndex: 0
                        }

                        Label {
                            text: qsTr("Longitude [°]")
                        }

                        TextField {
                            id: longitudeInput
                            text: Number(19.04804).toLocaleString(Qt.locale(), "f", 5)
                            maximumLength: 9
                            validator: DoubleValidator {
                                bottom: 0
                                top: 180
                                decimals: 5
                                notation: DoubleValidator.StandardNotation
                            }
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Your latitude or longitude is never sent to our server through this application!")
                        }

                        ComboBox {
                            id: longitudeCombo
                            model: ['West','East']
                            currentIndex: 1
                        }

                        Label {
                            text: qsTr("Altitude [m]")
                        }

                        TextField {
                            id: elevationInput
                            validator: IntValidator{
                                top: 10000
                                bottom: -10000
                            }
                        }

                        // Spacer for the grid
                        Item {
                            width: 10
                            height: 10
                        }

                        Label {
                            text: qsTr("Satellite ID")
                        }

                        TextField {
                            id: satIdInput
                            maximumLength: 6
                            validator: IntValidator{
                                bottom: 0
                                top: 999999
                            }
                            onEditingFinished: {
                                predicterController.changeSatID(parseInt(satIdInput.text))
                            }
                        }
                    }

                    Column {
                        spacing: parent.spacing
                        enabled: predicterController.tracking

                        Grid{
                            spacing: compactLayout ? parent.spacing : 30
                            columns: compactLayout ? 1 : 2
                            verticalItemAlignment: Grid.AlignVCenter

                            Row {
                                spacing: 5
                                Label {
                                    font.pixelSize: 20
                                    text: qsTr("Azimuth [°]:")
                                }
                                Label {
                                    id: predictAzText
                                    font.pixelSize: 20
                                    text: qsTr("N/A")
                                }
                            }
                            Row{
                                spacing: 5
                                Label {
                                    font.pixelSize: 20
                                    text: qsTr("Elevation [°]:")
                                }
                                Label {
                                    font.pixelSize: 20
                                    text: qsTr("N/A")
                                    id: predictElText
                                }
                            }
                        }
                        Row{
                            spacing: 5
                            Label {
                                font.pixelSize: 20
                                text: qsTr("Doppler@" + (baseFreq / 1000000) + " Mhz [Hz]:")
                            }
                            Label {
                                font.pixelSize: 20
                                id: predictDoppText
                                text: qsTr("N/A")
                            }
                        }
                        Row{
                            spacing: 50
                            Row{
                                spacing: 5
                                Label {
                                    text: qsTr("AOS:")
                                }
                                Label {
                                    id: predictAOSText
                                    text: qsTr("N/A")
                                }
                            }
                            Row{
                                spacing: 5
                                Label {
                                    text: qsTr("LOS:")
                                }
                                Label {
                                    id: predictLOSText
                                    text: qsTr("N/A")
                                }
                            }
                        }
                    }
                }
            }

            Connections {
                target: predicterController

                onTleErrorSignal: {
                    trackingNOTLE.open();
                    trackingSwitch.checked = false;
                    logger.writeToLog("Tracking error: Tracking could not be started without TLE data.");
                }
                onNeverVisibleSignal: {
                    trackingNotVisible.open();
                    trackingSwitch.checked = false;
                    logger.writeToLog("Tracking error: The satellite is never visible from the location specified.");
                }
                onDecayedSignal: {
                    trackingDecayed.open();
                    trackingSwitch.checked = false;
                    logger.writeToLog("Tracking error: The satellite appears to have decayed.");
                }
                onTrackingDataSignal: {
                    predictAzText.text = azimuth.toFixed(1);
                    predictElText.text = elevation.toFixed(1);
                    predictDoppText.text = (doppler100*baseFreq/100000000).toFixed(0);
                    predictAOSText.text = nextAOSQS;
                    predictLOSText.text = nextLOSQS;
                }
            }
        }

        GroupBox {
            id: radioGroupBox
            title: qsTr("Radio")
            width: parent.width

            Column {
                spacing: 10

                Switch {
                    id: radioSwitch
                    enabled: (predicterController.tracking && radioCOMCombo.count > 0)
                    text: enabled? qsTr("Control a radio"): '<font color="red">Radio controlling is only allowed if tracking is running and a radio is connected</font>'
                    onCheckedChanged: {
                        if (checked){
                            // turning the radio on
                            if (radioModelCombo.currentIndex === 0) {
                                //ft817
                                ft817.start(String(radioCOMCombo.currentText), radioBaudRate.currentText, radioOffsetSpinbox.value, ft817OnOffSwitch.checked);
                            } else if (radioModelCombo.currentIndex === 1) {
                                //ts2000
                                ts2000.start(String(radioCOMCombo.currentText), radioBaudRate.currentText, radioOffsetSpinbox.value);
                            } else if (radioModelCombo.currentIndex === 2) {
                                // smog radio
                                smogradio.set5VOut(smogRadio5VOutSwitch.checked)
                                smogradio.start(String(radioCOMCombo.currentText), 115200, radioOffsetSpinbox.value, true);
                            } else if (radioModelCombo.currentIndex === 3) {
                                //ft847
                                ft847.start(String(radioCOMCombo.currentText), radioBaudRate.currentText, radioOffsetSpinbox.value, false);
                            } else if (radioModelCombo.currentIndex === 4) {
                                //ft991
                                ft991.start(String(radioCOMCombo.currentText), radioBaudRate.currentText, radioOffsetSpinbox.value);
                            } else if (radioModelCombo.currentIndex === 5) {
                                //icom
                                icom.start(String(radioCOMCombo.currentText), radioBaudRate.currentText, radioOffsetSpinbox.value);
                            }
                        } else {
                            // turning the radio off
                            if (radioModelCombo.currentIndex === 0) {
                                //ft817
                                ft817.stop();
                            } else if (radioModelCombo.currentIndex === 1) {
                                //ts2000
                                ts2000.stop();
                            } else if (radioModelCombo.currentIndex === 2) {
                                // smog radio
                                smogradio.stop();
                            } else if (radioModelCombo.currentIndex === 3) {
                                // ft847 radio
                                ft847.stop();
                            } else if (radioModelCombo.currentIndex === 4) {
                                // ft991 radio
                                turnOnOff = true;
                                ft991.stop();
                            } else if (radioModelCombo.currentIndex === 5) {
                                // icom radio
                                icom.stop();
                            }
                        }
                    }
                    onEnabledChanged: {
                        if (!enabled){
                            checked = false;
                        }
                    }
                }

                Column {
                    x: radioSwitch.indicator.width + radioSwitch.padding * 2
                    spacing: parent.spacing

                    Grid {
                        enabled: !radioSwitch.checked && (radioCOMCombo.count > 0)
                        spacing: parent.spacing
                        columns: 2
                        verticalItemAlignment: Grid.AlignVCenter

                        Label {
                            text: qsTr("Model")
                        }

                        ComboBox {
                            id: radioModelCombo
                            model: ['FT-817','TS-2000','SMOG','FT-847','FT-991','ICOM']
                            currentIndex: 0
                        }


                    }

                    Grid {
                        enabled: !radioSwitch.checked && (radioCOMCombo.count > 0)
                        spacing: parent.spacing
                        columns: compactLayout ? 4 : 8
                        verticalItemAlignment: Grid.AlignVCenter


                        Label {
                            text: qsTr("COM port")
                        }

                        ComboBox {
                            id: radioCOMCombo
                            model: deviceDiscovery.serialPorts
                            textRole: "display"
                        }

                        Label {
                            text: qsTr("Baud rate")
                            visible: (radioModelCombo.currentIndex !== 2) // smogradio only supports 115200
                        }

                        ComboBox {
                            id: radioBaudRate
                            model: ['4800','9600','19200','38400']
                            visible: (radioModelCombo.currentIndex !== 2) // smogradio only supports 115200
                        }

                        Label {
                            text: qsTr("Offset [Hz]")
                        }

                        SpinBox {
                            id:  radioOffsetSpinbox
                            from: -25000
                            to: 25000
                            value: 0
                            stepSize: 5
                        }

                        CheckBox {
                            id: ft817OnOffSwitch
                            visible: (radioModelCombo.currentIndex === 0)
                            enabled: visible && !radioSwitch.checked && (radioCOMCombo.count > 0)
                            text: qsTr("Turn on and off")
                            checked: false
                        }

                        Switch {
                            id: smogRadio5VOutSwitch
                            visible: (radioModelCombo.currentIndex === 2) // only for smog radio
                            text: "5V out"
                            ToolTip.delay: 1000
                            ToolTip.timeout: 5000
                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Send 5V DC to the preamplifier")

                        }
                    }
                }
            }

            Connections {
                target: ft817
                onSerialPortErrorSignal:{
                    serialErrorRadio.open();
                    logger.writeToLog("Radio error: An error has occurred during communcation through the COM port.");
                    radioSwitch.checked = false;
                }
            }

            Connections {
                target: ft847
                onSerialPortErrorSignal:{
                    serialErrorRadio.open();
                    logger.writeToLog("Radio error: An error has occurred during communcation through the COM port.");
                    radioSwitch.checked = false;
                }
            }
            Connections {
                target: ft991
                onSerialPortErrorSignal:{
                    serialErrorRadio.open();
                    logger.writeToLog("Radio error: An error has occurred during communcation through the COM port.");
                    radioSwitch.checked = false;
                }
            }
            Connections {
                target: icom
                onSerialPortErrorSignal:{
                    serialErrorRadio.open();
                    logger.writeToLog("Radio error: An error has occurred during communcation through the COM port.");
                    radioSwitch.checked = false;
                }
            }
            Connections {
                target: smogradio
                onSerialPortErrorSignal:{
                    serialErrorRadio.open();
                    logger.writeToLog("Radio error: An error has occurred during communcation through the COM port.");
                    radioSwitch.checked = false;
                }
            }
            Connections {
                target: ts2000
                onSerialPortErrorSignal:{
                    serialErrorRadio.open();
                    logger.writeToLog("Radio error: An error has occurred during communcation through the COM port.");
                    radioSwitch.checked = false;
                }
            }
        }

        GroupBox {
            id: rotatorGroupBox
            title: qsTr("Rotator")
            width: parent.width

            Column {
                spacing: 10

                Switch{
                    id: rotatorSwitch
                    enabled: (predicterController.tracking && rotatorCOMCombo.count > 0)
                    text: enabled? qsTr("Control an antenna rotator"): '<font color="red">Antenna rotator controlling is only allowed if tracking is running and a rotator is connected</font>'
                    checked: false
                    onCheckedChanged: {
                        if (checked){
                            g5500.start(rotatorCOMCombo.currentText, rotatorBaudRate.currentText, parkingAZSpin.value, parkingELSpin.value, rotatorAzimuthStopSpin.value, (rotatorMaxElev.currentIndex === 1), rotatorParkingSwitch.checked);
                        } else {
                            g5500.stop();
                        }
                    }
                    onEnabledChanged: {
                        if (!enabled){
                            checked = false;
                        }
                    }
                }

                Column {
                    x: rotatorSwitch.indicator.width + rotatorSwitch.padding * 2
                    spacing: parent.spacing

                    Row {
                        spacing: parent.spacing

                        CheckBox {
                            id: rotatorParkingSwitch
                            text: qsTr("Park between passes")
                            checked: false
                            enabled: !rotatorSwitch.checked && (rotatorCOMCombo.count > 0)
                        }
                    }

                    Row {
                        spacing: parent.spacing
                        enabled: !rotatorSwitch.checked && (rotatorCOMCombo.count > 0)

                        Grid {
                            spacing: parent.spacing
                            columns: 2
                            verticalItemAlignment: Grid.AlignVCenter

                            Label {
                                text: qsTr("Model")
                            }

                            ComboBox{
                                id: rotatorModelCombo
                                model: ['G5500']
                            }

                            Label {
                                text: qsTr("COM port")
                            }

                            ComboBox {
                                id: rotatorCOMCombo
                                model: deviceDiscovery.serialPorts
                                textRole: "display"
                            }

                            Label {
                                text: qsTr("Baud rate")
                            }

                            ComboBox{
                                id: rotatorBaudRate
                                model: ['1200','2400','4800','9600']
                            }
                        }

                        Grid {
                            spacing: parent.spacing
                            columns: compactLayout ? 2 : 4
                            verticalItemAlignment: Grid.AlignVCenter

                            Label {
                                text: qsTr("Maximum elevation [°]")
                            }

                            ComboBox {
                                id: rotatorMaxElev
                                model: ['90','180']
                            }

                            Label {
                                text: qsTr("Azimuth stops at [°]")
                            }

                            SpinBox {
                                id: rotatorAzimuthStopSpin
                                from: -1
                                to: 359
                                value: 0
                            }

                            Label {
                                text: qsTr("Parking azimuth [°]")
                            }

                            SpinBox {
                                id: parkingAZSpin
                                enabled: rotatorParkingSwitch.checked
                                value: 0
                                from: 0
                                to: 360
                            }

                            Label {
                                text: qsTr("Parking elevation [°]")
                            }

                            SpinBox{
                                id: parkingELSpin
                                enabled: rotatorParkingSwitch.checked
                                value: 0
                                from: 0
                                to: 90
                            }
                        }
                    }
                }
            }

            Connections{
                target: g5500
                onSerialPortErrorSignal:{
                    serialErrorRotator.open();
                    logger.writeToLog("Rotator error: An error has occurred during communcation through the COM port.");
                    rotatorSwitch.checked = false;
                }
            }
        }

        GroupBox {
            title: qsTr("Miscellaneous")
            width: parent.width

            Column {
                spacing: 10

                Row {
                    spacing: parent.spacing
                    visible: false // TODO: it doesn't work properly when not set to the default, so hidden for now

                    Label {
                        text: qsTr("New packet position")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RadioButton {
                        id: newPacketTopRadio
                        text: qsTr("Top")
                        checked: true
                        onCheckedChanged: if (checked) packetTableModel.insertAtEnd = false;
                    }

                    RadioButton {
                        id: newPacketBottomRadio
                        text: qsTr("Bottom")
                        checked: false
                        onCheckedChanged: if (checked) packetTableModel.insertAtEnd = true;
                    }
                }

                Row {
                    spacing: parent.spacing

                    Label {
                        text: qsTr("Save settings on exit")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RadioButton {
                        id: alwaysRadio
                        text: qsTr("Always")
                        onCheckedChanged : if (checked) saveSettingsOnExit = 0;
                    }

                    RadioButton {
                        id: neverRadio
                        text: qsTr("Never")
                        onCheckedChanged : if (checked) saveSettingsOnExit = 1;
                    }

                    RadioButton {
                        id: askRadio
                        text: qsTr("Ask")
                        checked: true
                        onCheckedChanged : if (checked) saveSettingsOnExit = 2;
                    }
                }

                Row {
                    spacing: parent.spacing

                    Label {
                        text: qsTr("Satellite")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    ComboBox {
                        readonly property string smog1: "SMOG-1"
                        readonly property string smogp: "SMOG-P"
                        readonly property string atl1: "ATL-1"
                        id: satelliteSelectorCombo
                        enabled:  !soundcardInReceivePackets.checked && !sdrEnabledSwitch.checked && !trackingSwitch.checked
                        model: [smog1, smogp, atl1]
                        onCurrentIndexChanged: {
                            mainWindow.currentSatellite = currentIndex
                            switch (currentIndex){
                            case model.indexOf(smog1):
                                satelliteChanger.changeToSMOG1()
                                mainWindow.title = "SMOG-1 GND Client Software"
                                break;
                            case model.indexOf(smogp):
                                satelliteChanger.changeToSMOGP()
                                mainWindow.title = "SMOG-P GND Client Software"
                                satIdInput.text = "44832"
                                predicterController.changeSatID(44832)
                                break;
                            case model.indexOf(atl1):
                                satelliteChanger.changeToATL1()
                                mainWindow.title = "ATL-1 GND Client Software"
                                satIdInput.text = "44830"
                                predicterController.changeSatID(44830)
                                break;
                            }
                        }

                        Connections {
                            target: satelliteChanger
                            onNewBaseFrequency:{
                                baseFreq = newFrequencyHz
                                console.log("Base frequency has been changed to " + baseFreq + " Hz.")
                            }
                        }
                    }
                }
            }
        }

        GroupBox {
            id: uploadGroupBox
            width: parent.width
            title: qsTr("Upload")

            Column {
                spacing: 10

                Row {
                    spacing: parent.spacing

                    Label {
                        text: qsTr("Username")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TextField {
                        id: uploadUsernameTF
                        enabled: !uploader.isLoggedIn && !uploader.isCurrentlyLoggingIn
                        maximumLength: 64
                        anchors.verticalCenter: parent.verticalCenter
                        selectByMouse: true
                    }

                    Label {
                        text: qsTr("Password")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TextField {
                        id: uploadPasswordTF
                        enabled: !uploader.isLoggedIn && !uploader.isCurrentlyLoggingIn
                        maximumLength: 128
                        anchors.verticalCenter: parent.verticalCenter
                        selectByMouse: true
                        echoMode: TextInput.PasswordEchoOnEdit
                    }

                    Button {
                        id: loginButton
                        text: qsTr("Login")
                        enabled: !uploader.isLoggedIn && !uploader.isCurrentlyLoggingIn && uploadUsernameTF.text.length > 0 && uploadPasswordTF.text.length > 0
                        onClicked: {
                            uploader.login(uploadUsernameTF.text.trim(), uploadPasswordTF.text)
                        }
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Button {
                        id: logoutButton
                        text: qsTr("Logout")
                        enabled: uploader.isLoggedIn || uploader.isCurrentlyLoggingIn
                        onClicked: {
                            uploader.logout()
                        }
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    CheckBox {
                        id: autoLoginSwitch
                        text: qsTr("Auto login")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    BusyIndicator {
                        running: uploader.isCurrentlyLoggingIn
                        height: loginButton.height + 5
                        width: height
                    }

                }

                Row {
                    spacing: parent.spacing

                    Label {
                        text: qsTr("Automatically upload at [packets]")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    SpinBox {
                        id: uploaderAUFSB
                        from: 5
                        to: 20
                        stepSize: 1
                        enabled: true
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: {
                            if (uploader.remaining >= uploaderAUFSB.value && uploader.isLoggedIn && !uploader.isCurrentlyUploading){
                                uploader.upload()
                            }
                        }
                    }

                    Button {
                        text: qsTr("Force upload")
                        enabled: uploader.isLoggedIn && !uploader.isCurrentlyUploading
                        onClicked: {
                            uploader.upload()
                        }
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            Connections {
                target: uploader
                onIsLoggedInChanged:{
                    if (uploader.isLoggedIn && !uploader.isCurrentlyUploading && uploader.remaining >= uploaderAUFSB.value){
                        uploader.upload()
                    }
                }
                onIsCurrentlyUploadingChanged: {
                    if (uploader.isLoggedIn && !uploader.isCurrentlyUploading && uploader.remaining >= uploaderAUFSB.value){
                        uploader.upload()
                    }
                }
                onRemainingChanged: {
                    if (uploader.isLoggedIn && !uploader.isCurrentlyUploading && uploader.remaining >= uploaderAUFSB.value){
                        uploader.upload()
                    }
                }
                onInvalidLoginCredentials:{
                    invalidLoginCredentialsError.open()
                }
                onServerDown:{
                    serverDownError.open()
                }
                onTimedOut:{
                    if (uploader.errorsInARow >= 10) {
                        timedOutError.open()
                    }
                }
            }
        }

        Dialogs1.MessageDialog {
            id: trackingNotVisible
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with tracking the satellite")
            text: qsTr("It appears that the satellite is never visible from your station.\n
                    Try updating the TLE and changing the parameters of your station.")
        }
        Dialogs1.MessageDialog {
            id: trackingDecayed
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with tracking the satellite")
            text: qsTr("It appears that the satellite has decayed.\nTry updating the TLE.")
        }
        Dialogs1.MessageDialog {
            id: trackingNOTLE
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with tracking the satellite")
            text: qsTr("We load the satellite's TLE from http://www.celestrak.com/NORAD/elements/active.txt and couldn't load from a local file either."+
                       "\nIf the server is unreachable, rename one of the previous TLE files to \"manual.txt\" and try enabling tracking."+
                       "\n\nIt is possible that the satellite ID currently in use is not be valid. Set a new value, if that is the case!")
        }
        Dialogs1.MessageDialog {
            id: audioDeviceDisconnected
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with audio device")
            text: qsTr("The audio device has been disconnected.\nPlease verify that it is connected properly and retry the operation.")
        }
        Dialogs1.MessageDialog {
            id: audioDeviceFailedError
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with sound card")
            text: qsTr("We couldn't start the audio input device.\nPlease verify that audio input is configured correctly on your computer and it supports 44100Hz, then retry the operation.")
        }
        Dialogs1.MessageDialog {
            id: sdrErrorSDRDisconnected
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with SDR - SDR has been disconnected")
            text: qsTr("The SDR has been disconnected.\nPlease verify that it is connected properly and is not used by any other process.")
        }
        Dialogs1.MessageDialog {
            id: invalidSdrDeviceIndex
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with SDR - Invalid device")
            text: qsTr("You have attempted to start a non-existent SDR device. Please refresh the device list and try again.")
        }
        Dialogs1.MessageDialog {
            id: sdrErrorNoSDR
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with SDR - Couldn't start SDR")
            text: qsTr("We couldn't start the SDR.\nPlease verify that it is connected properly and is not used by any other process.\n" +
                       "You may need to install some drivers and configure the device.\n\n" +
                       "For windows, you can find a detailed howto at https://www.rtl-sdr.com/rtl-sdr-quick-start-guide/" +
                       "\n\nYou may also need to refresh the device list.")
        }
        Dialogs1.MessageDialog {
            id: sdrDeviceUnchosen
            icon: Dialogs1.StandardIcon.Critical
            title: qsTr("Error with SDR - No device chosen")
            text: qsTr("Please pick a device before starting the operation.")
        }
        Dialogs1.MessageDialog {
            id: invalidLongitude
            icon: Dialogs1.StandardIcon.Warning
            title: qsTr("Invalid longitude")
            text: qsTr("Only a value between 0 and 180 can be set for longitude.")
        }
        Dialogs1.MessageDialog {
            id: invalidLatitude
            icon: Dialogs1.StandardIcon.Warning
            title: qsTr("Invalid latitude")
            text: qsTr("Only a value between 0 and 90 can be set for latitude.")
        }
        Dialogs1.MessageDialog {
            id: serialErrorRadio
            icon: Dialogs1.StandardIcon.Warning
            title: qsTr("Serial port error")
            text: qsTr("An error has occurred during the communcation with the radio serial port."+
                       "\nThe communication has been terminated."+
                       "\nPlease make sure that the radio and the port are connected properly, and that the port is not being used.")
        }
        Dialogs1.MessageDialog {
            id: serialErrorRotator
            icon: Dialogs1.StandardIcon.Warning
            title: qsTr("Serial port error")
            text: qsTr("An error has occurred during the communcation with the rotator serial port."+
                       "\nThe communication has been terminated."+
                       "\nPlease make sure that the rotator and the port are connected properly, and that the port is not being used.")
        }
        Dialogs1.MessageDialog {
            id: invalidLoginCredentialsError
            icon: Dialogs1.StandardIcon.Warning
            title: qsTr("Invalid credentials")
            text: qsTr("You have provided an invalid username/password pair!")
        }
        Dialogs1.MessageDialog {
            id: serverDownError
            icon: Dialogs1.StandardIcon.Warning
            title: qsTr("Our server is down")
            text: qsTr("Our server appears to be down.\n\nIf the problem persists, please get in contact with us!")
        }
        Dialogs1.MessageDialog {
            id: timedOutError
            icon: Dialogs1.StandardIcon.Warning
            title: qsTr("Operation timed out")
            text: qsTr("The operation towards our server has timed out.\nPlease ensure the problem is not on your end and try again."+
                       "\n\nIf you believe the issue is on our end, please get in contact with us!")
        }
    }
}
