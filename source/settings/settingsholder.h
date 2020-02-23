#ifndef SETTINGSHOLDER_H
#define SETTINGSHOLDER_H

#include <QObject>
#include <QString>

// macro for creating properties with minimal effort
#define GENPROPERTY(type, name)                      \
private:                                             \
    type name##_priv;                                \
                                                     \
public:                                              \
    inline const type &name() const {                \
        return name##_priv;                          \
    }                                                \
    inline void set_##name(const type &value) {      \
        name##_priv = value;                         \
    }                                                \
    Q_PROPERTY(type name READ name WRITE set_##name) \
private:

/**
 * @brief A class that encompasses every setting.
 *
 * Its properties are writtend from QML when saving and then persisted by using SettingsProxy from C++.
 * The properties are grouped in the source code.
 */
class SettingsHolder : public QObject {
    Q_OBJECT

    // Soundcard settings group
    // settings in order: deviceName
    GENPROPERTY(QString, sdn)

    // Tracking settings group
    // settings in order: satelliteId, stationLat, stationLon, stationElev
    GENPROPERTY(qlonglong, tsi)
    GENPROPERTY(QString, tsla)
    GENPROPERTY(QString, tslo)
    GENPROPERTY(int, tse)

    // SDR settings group
    // settings in order: offsets, PPM, gain, automaticDFTracking, DF
    GENPROPERTY(QString, sdroffs)
    GENPROPERTY(int, sdrppm)
    GENPROPERTY(int, sdrgain)
    GENPROPERTY(bool, sdradft)

    // Radio settings group
    // settings in order: COMPort, baudRate, offsets, model, shouldRadioTurnOn, smogRadio5VOutSwitchOn
    GENPROPERTY(QString, racp)
    GENPROPERTY(int, rabr)
    GENPROPERTY(QString, raoffs)
    GENPROPERTY(QString, ram)
    GENPROPERTY(bool, rasrto)
    GENPROPERTY(bool, rasr5voso)

    // Rotator settings group
    // settings in order: COMPort, baudRate, model, canDo180Elev, azStopPoint, shouldPark, parkingAz,
    // parkingElev
    GENPROPERTY(QString, rocp)
    GENPROPERTY(int, robr)
    GENPROPERTY(QString, rom)
    GENPROPERTY(bool, rocde)
    GENPROPERTY(int, roazsp)
    GENPROPERTY(bool, rosp)
    GENPROPERTY(int, ropaz)
    GENPROPERTY(int, ropelev)

    // Misc settings group
    // settings in order: newPacketsAtEnd, saveSettingsOnExit, satelliteIndex
    GENPROPERTY(bool, mnpae)
    GENPROPERTY(int, mssoe)
    GENPROPERTY(int, msatidx)

    // Upload settings group
    // settings in order: username, password, storePassword, autoLogin, automaticUploadFrequency
    GENPROPERTY(QString, uu)
    GENPROPERTY(QString, up)
    GENPROPERTY(bool, usp)
    GENPROPERTY(bool, ual)
    GENPROPERTY(int, uauf)

public:
    /**
     * @brief Constructor, that realizes nothing special, initializes \p parent.
     * @param[in] parent Parent QObject, should be left empty.
     */
    explicit SettingsHolder(QObject *parent = 0);

signals:

public slots:
};

#endif // SETTINGSHOLDER_H
