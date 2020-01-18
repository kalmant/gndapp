#include "satellitechanger.h"

SatelliteChanger::SatelliteChanger(QObject *parent) : QObject(parent) {
    currentSatellite_priv = Satellites::UNSET;
}

QString SatelliteChanger::satelliteName() const {
    switch (currentSatellite_priv) {
    default:
    case Satellites::SMOG1:
        return QStringLiteral("SMOG-1");
    case Satellites::SMOGP:
        return QStringLiteral("SMOG-P");
    case Satellites::ATL1:
        return QStringLiteral("ATL-1");
    }
}

void SatelliteChanger::changeToSMOG1() {
    if (currentSatellite_priv != Satellites::SMOG1) {
        currentSatellite_priv = Satellites::SMOG1;
        emit satelliteChanged();
        emit newBaseFrequency(437345000);
        emit newFilePrefix("smog1");
        emit newSpectrumTitle("SMOG-1");
        emit newSatellite(Satellites::SMOG1);
        emit newSDRConfiguration(437332500, 12500);
    }
}

void SatelliteChanger::changeToSMOGP() {
    if (currentSatellite_priv != Satellites::SMOGP) {
        currentSatellite_priv = Satellites::SMOGP;
        emit satelliteChanged();
        emit newBaseFrequency(437150000);
        emit newFilePrefix("smogp");
        emit newSpectrumTitle("SMOG-P");
        emit newSatellite(Satellites::SMOGP);
        emit newSDRConfiguration(437162500, -12500);
    }
}

void SatelliteChanger::changeToATL1() {
    if (currentSatellite_priv != Satellites::ATL1) {
        currentSatellite_priv = Satellites::ATL1;
        emit satelliteChanged();
        emit newBaseFrequency(437175000);
        emit newFilePrefix("atl1");
        emit newSpectrumTitle("ATL-1");
        emit newSatellite(Satellites::ATL1);
        emit newSDRConfiguration(437162500, 12500);
    }
}
