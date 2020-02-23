#ifndef SETTINGSPROXY_H
#define SETTINGSPROXY_H

#include "settingsholder.h"
#include <QDebug>
#include <QObject>
#include <QSettings>

// Saves and loads settings
/**
 * @brief The class that handles persistent setting storage.
 */
class SettingsProxy : public QObject {
    Q_OBJECT
public:
    explicit SettingsProxy(SettingsHolder *settingsHolder, QObject *parent = 0);

    // Saves the values stored in settingsHolder
    Q_INVOKABLE bool saveSettings();

    // Loads the settings and emits them. The signals are then processed in QML.
    Q_INVOKABLE bool loadSettings(bool emits = true);

private:
    SettingsHolder *sh; //!< The pointer to the SettingsHolder object that makes saving of settings from QML possible.
    QSettings settings; //!< The QSettings object that accesses, saves and loads the persistent storage.
signals:
    /**
     * @brief Signal, that emits the soundcard settings after loading it from persistent storage.
     * @param deviceName Name of audio device that should be selected.
     */
    void loadSoundcardSettings(QString deviceName);

    /**
     * @brief Signal, that emits the SDR settings after loading it from persistent storage.
     * @param offset The offset frequency for the SDR.
     * @param PPM The PPM setting for the SDR. It is saved as PPM hundredths (1.43 -> 143)
     * @param gain The gain setting for the SDR.
     * @param autoDFTrack True, if the automatic doppler frequency correction should be enabled.
     */
    void loadSDRSettings(int offset, int PPM, int gain, bool autoDFTrack);

    /**
     * @brief Signal, that emits the tracking settings after loading it from persistent storage.
     * @param qlonglong The ID of the satellite
     * @param stationLat The station's location's latitude in degrees.
     * @param stationLong The station's location's longitude in degrees.
     * @param stationElev The station's location's altitude in meters.
     */
    void loadTrackingSettings(qlonglong satelliteId, QString stationLat, QString stationLong, int stationElev);

    /**
     * @brief Signal, that emits the radio settings after loading it from persistent storage.
     * @param COMPort Name of the COM port used by the radio.
     * @param baudRate Baud rate used for the communcation on the COM port.
     * @param offset The offset frequency for the radio.
     * @param model Name of the radio model that should be set.
     * @param shouldRadioTurnOn True, if the radio should turn on between passes of the satelite. FT-817 specific
     * setting.
     * @param smogRadio5VOutSwitchOn True, if smogRadio5VOutSwitch should be on
     */
    void loadRadioSettings(
        QString COMPort, int baudRate, int offset, QString model, bool shouldRadioTurnOn, bool smogRadio5VOutSwitchOn);

    /**
     * @brief Signal, that emits the rotator settings after loading it from persistent storage.
     * @param COMPort Name of the COM port used by the rotator.
     * @param baudRate Baud rate used for the communcation on the COM port.
     * @param model Name of the rotator model that should be set.
     * @param canDo180Elev True, if the rotator can turn to 180 degrees elevation.
     * @param azStopPoint The point, which the rotator can not turn past during a pass.
     * @param shouldPark True, if the rotator should go to parking position between passes.
     * @param parkingAz The azimuth where the rotator should park.
     * @param parkingElev The elevation where the rotator should park.
     */
    void loadRotatorSettings(QString COMPort,
        int baudRate,
        QString model,
        bool canDo180Elev,
        int azStopPoint,
        bool shouldPark,
        int parkingAz,
        int parkingElev);

    /**
     * @brief Signal, that emits the miscellaneous settings after loading it from persistent storage.
     * @param newPacketsAtEnd True, if packets should be added to the end of the list, instead of the start.
     * @param saveSettingsOnExit 0 if settings should always save on exit, 1 if it should never save and 2 if there
     * should be a popup asking for this decision.
     * @param satelliteIndex id of the satellite currently in use
     */
    void loadMiscSettings(bool newPacketsAtEnd, int saveSettingsOnExit, int satelliteIndex);

    /**
     * @brief Signal, that emits the upload settings after loading it from persistent storage.
     * @param username Username
     * @param password Password
     * @param storePassword Whether the password should be stored
     * @param autoLogin Whether login should be attempted after on startup
     * @param automaticUploadFrequency The number of packets that trigger an upload automatically
     */
    void loadUploadSettings(
        QString username, QString password, bool storePassword, bool autoLogin, int automaticUploadFrequency);
};

#endif // SETTINGSPROXY_H
