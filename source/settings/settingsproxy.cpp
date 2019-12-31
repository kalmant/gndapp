#include "settingsproxy.h"

static QString encrypt(const QString& input) {
    QByteArray output;
    output.reserve(input.count());
    for (auto itr = input.begin(); itr != input.end(); ++itr)
        output.append(~itr->toLatin1());
    return output.toBase64();
}

static QString decrypt(const QString& input) {
    QByteArray array = QByteArray::fromBase64(input.toLatin1());
    QString output;
    output.reserve(array.count());
    for (auto itr = array.begin(); itr != array.end(); ++itr)
        output.append(~*itr);
    return output;
}

/**
 * @brief Constructor for the class. Initializes \p sh.
 * @param[in] settingsHolder Pointer to the SettingsHolder that allows saving of settings from QML.
 * @param parent The parent QObject, should be left empty.
 */
SettingsProxy::SettingsProxy(SettingsHolder *settingsHolder, QObject *parent) : QObject(parent) {
    sh = settingsHolder;
    loadSettings(false);
}

/**
 * @brief Saves every relevant setting persistently.
 * @return Returns true upon successful saving.
 */
bool SettingsProxy::saveSettings() {
    settings.beginGroup("Waterfall");
    settings.setValue("deviceName", sh->wdn());
    settings.setValue("adaptiveColoring", sh->wac());
    settings.setValue("scalingFactor", sh->wsf());
    settings.setValue("sampleCount", sh->wsc());
    qInfo() << "Successfully saved Waterfall settings";
    settings.endGroup();

    settings.beginGroup("Soundcard");
    settings.setValue("showWaterfallOnStartup", sh->sswos());
    qInfo() << "Successfully saved Soundcard settings";
    settings.endGroup();

    settings.beginGroup("Tracking");
    settings.setValue("satelliteId", sh->tsi());
    settings.setValue("baseFrequency", sh->tbf());
    settings.setValue("stationLat", sh->tsla());
    settings.setValue("stationLon", sh->tslo());
    settings.setValue("stationElev", sh->tse());
    qInfo() << "Successfully saved Tracking settings";
    settings.endGroup();

    settings.beginGroup("SDR");
    settings.setValue("offset", sh->sdroffs());
    settings.setValue("PPM", sh->sdrppm());
    settings.setValue("gain", sh->sdrgain());
    settings.setValue("automaticDFTracking", sh->sdradft());
    settings.setValue("DF", sh->sdrdf());
    qInfo() << "Successfully saved SDR settings";
    settings.endGroup();

    settings.beginGroup("Radio");
    settings.setValue("COMPort", sh->racp());
    settings.setValue("baudRate", sh->rabr());
    settings.setValue("offset", sh->raoffs());
    settings.setValue("model", sh->ram());
    settings.setValue("shouldRadioTurnOn", sh->rasrto());
    settings.setValue("smogRadio5VOutSwitchOn", sh->rasr5voso());
    qInfo() << "Successfully saved Radio settings";
    settings.endGroup();

    settings.beginGroup("Rotator");
    settings.setValue("COMPort", sh->rocp());
    settings.setValue("baudRate", sh->robr());
    settings.setValue("model", sh->rom());
    settings.setValue("canDo180Elev", sh->rocde());
    settings.setValue("azStopPoint", sh->roazsp());
    settings.setValue("shouldPark", sh->rosp());
    settings.setValue("parkingAz", sh->ropaz());
    settings.setValue("parkingElev", sh->ropelev());
    qInfo() << "Successfully saved Rotator settings";
    settings.endGroup();

    settings.beginGroup("Misc");
    settings.setValue("newPacketsAtEnd", sh->mnpae());
    settings.setValue("saveSettingsOnExit", sh->mssoe());
    settings.setValue("satelliteIndex", sh->msatidx());
    qInfo() << "Successfully saved Misc settings";
    settings.endGroup();

    settings.beginGroup("Upload");
    settings.setValue("username", sh->uu());
    settings.setValue("password", encrypt(sh->up()));
    settings.setValue("automaticLogin", sh->ual());
    settings.setValue("automaticUploadFrequency", sh->uauf());
    qInfo() << "Successfully saved Upload settings";
    settings.endGroup();

    settings.sync();
    qInfo() << "Successfully saved every setting";

    return true;
}

/**
 * @brief Loads every relevant setting from persistent storage.
 *
 * After loading the settings, it emits SettingsProxy::loadWaterfallSettings(), SettingsProxy::loadSoundcardSettings(),
 * etc..
 * @param emits emits signals if \p emits is true
 *
 * @return Returns true upon successful loading.
 */
bool SettingsProxy::loadSettings(bool emits) {
    settings.sync();
    settings.beginGroup("Waterfall");
    sh->set_wdn(settings.value("deviceName", "").toString());
    sh->set_wac(settings.value("adaptiveColoring", true).toBool());
    sh->set_wsf(settings.value("scalingFactor", 100).toInt());
    sh->set_wsc(settings.value("sampleCount", 4096).toInt());
    if (emits) {
        emit loadWaterfallSettings(sh->wdn(), sh->wac(), sh->wsf(), sh->wsc());
        qInfo() << "Emitted Waterfall settings";
    }
    settings.endGroup();

    settings.beginGroup("Soundcard");
    sh->set_sswos(settings.value("showWaterfallOnStartup", true).toBool());
    if (emits) {
        emit loadSoundcardSettings(sh->sswos());
        qInfo() << "Emitted Soundcard settings";
    }
    settings.endGroup();

    settings.beginGroup("Tracking");
    sh->set_tsi(settings.value("satelliteId", 44832).toLongLong());
    sh->set_tbf(settings.value("baseFrequency", 437150000).toLongLong());
    sh->set_tsla(settings.value("stationLat", "47,48675").toString());
    sh->set_tslo(settings.value("stationLon", "-19,04804").toString());
    sh->set_tse(settings.value("stationElev", 275).toInt());
    if (emits) {
        emit loadTrackingSettings(sh->tsi(), sh->tbf(), sh->tsla(), sh->tslo(), sh->tse());
        qInfo() << "Emitted Tracking settings";
    }
    settings.endGroup();

    settings.beginGroup("SDR");
    sh->set_sdroffs(settings.value("offset", 0).toInt());
    sh->set_sdrppm(settings.value("PPM", 0).toInt());
    sh->set_sdrgain(settings.value("gain", 0).toInt());
    sh->set_sdradft(settings.value("automaticDFTracking", false).toBool());
    sh->set_sdrdf(settings.value("DF", 0).toInt());
    if (emits) {
        emit loadSDRSettings(sh->sdroffs(), sh->sdrppm(), sh->sdrgain(), sh->sdradft(), sh->sdrdf());
        qInfo() << "Emitted SDR settings";
    }
    settings.endGroup();

    settings.beginGroup("Radio");
    sh->set_racp(settings.value("COMPort", "COM 6").toString());
    sh->set_rabr(settings.value("baudRate", 4800).toInt());
    sh->set_raoffs(settings.value("offset", 0).toInt());
    sh->set_ram(settings.value("model", "FT-817").toString());
    sh->set_rasrto(settings.value("shouldRadioTurnOn", false).toBool());
    sh->set_rasr5voso(settings.value("smogRadio5VOutSwitchOn", false).toBool());
    if (emits) {
        emit loadRadioSettings(sh->racp(), sh->rabr(), sh->raoffs(), sh->ram(), sh->rasrto(), sh->rasr5voso());
        qInfo() << "Emitted Radio settings";
    }
    settings.endGroup();

    settings.beginGroup("Rotator");
    sh->set_rocp(settings.value("COMPort", "COM 6").toString());
    sh->set_robr(settings.value("baudRate", 4800).toInt());
    sh->set_rom(settings.value("model", "G5500").toString());
    sh->set_rocde(settings.value("canDo180Elev", false).toBool());
    sh->set_roazsp(settings.value("azStopPoint", 0).toInt());
    sh->set_rosp(settings.value("shouldPark", false).toBool());
    sh->set_ropaz(settings.value("parkingAz", 0).toInt());
    sh->set_ropelev(settings.value("parkingElev", 90).toInt());
    if (emits) {
        emit loadRotatorSettings(
            sh->rocp(), sh->robr(), sh->rom(), sh->rocde(), sh->roazsp(), sh->rosp(), sh->ropaz(), sh->ropelev());
        qInfo() << "Emitted Rotator settings";
    }
    settings.endGroup();

    settings.beginGroup("Misc");
    sh->set_mnpae(settings.value("newPacketsAtEnd", false).toBool());
    sh->set_mssoe(settings.value("saveSettingsOnExit", 2).toInt());
    sh->set_msatidx(settings.value("satelliteIndex", 1).toInt());
    if (emits) {
        emit loadMiscSettings(sh->mnpae(), sh->mssoe(), sh->msatidx());
        qInfo() << "Emitted Misc settings";
    }
    settings.endGroup();

    settings.beginGroup("Upload");
    sh->set_uu(settings.value("username", "username").toString());
    sh->set_up(decrypt(settings.value("password", "password").toString()));
    sh->set_ual(settings.value("automaticLogin", false).toBool());
    sh->set_uauf(settings.value("automaticUploadFrequency", 5).toInt());
    if (emits) {
        emit loadUploadSettings(sh->uu(), sh->up(), sh->ual(), sh->uauf());
        qInfo() << "Emitted Upload settings";
    }
    settings.endGroup();

    return true;
}
