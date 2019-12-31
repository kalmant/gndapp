#ifndef SATELLITECHANGER_H
#define SATELLITECHANGER_H

#include <QObject>

class SatelliteChanger : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString satelliteName READ satelliteName NOTIFY satelliteChanged)

public:
    enum class Satellites {
        SMOG1 = 0,
        SMOGP = 1,
        ATL1 = 2,
        UNSET = 322,
    };

    Q_ENUM(Satellites)

    explicit SatelliteChanger(QObject *parent = nullptr);
    QString satelliteName() const;

    Q_INVOKABLE void changeToSMOG1();
    Q_INVOKABLE void changeToSMOGP();
    Q_INVOKABLE void changeToATL1();
    Q_INVOKABLE void changeToOther();

private:
    Satellites currentSatellite_priv;

signals:
    void satelliteChanged();

    /**
     * @brief The signal that is used to communicate that the base frequency has changed
     * @param newFrequencyHz The new base frequency
     */
    void newBaseFrequency(unsigned long newFrequencyHz);

    void newSatelliteId(unsigned int newSatelliteId);

    /**
     * @brief The signal that is used to communicate that the satellite filename prefix has changed
     * @param prefix The new prefix
     */
    void newFilePrefix(QString newPrefix);

    void newSatellite(Satellites satellite);

    /**
     * @brief This signal tells the SpectrumReceiver window to change its title.
     * @param satelliteName The new satellite name to use in the spectrum receiver.
     */
    void newSpectrumTitle(QString satelliteName);
};

#endif // SATELLITECHANGER_H
