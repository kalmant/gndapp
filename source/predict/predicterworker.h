#ifndef PREDICTERWORKER_H
#define PREDICTERWORKER_H
#include "predictmod.h"
#include <QByteArray>
#include <QDateTime>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QScopedPointer>
#include <QThread>
#include <QTimer>

/**
 * @brief The class that wraps the implementation of tracking computations.
 *
 * Allows the setting of station coordinates and emits the tracking data.
 * Automatically tries to acquire TLE data from either NORAD or manual.txt.
 * Saves the acquired data to tleYYYYMMDDl.txt, where Y,M and D denote parts of the date and l specifies which half of
 * the day it was acquired on. Saves the current tracking data to trackingdata.txt. Latitude and longitude are stored as
 * degrees(double), altitude is stored as meters(int). Currently configured to AO-27 TLE data, but SMOG-1 frequencies.
 */
class PredicterWorker : public QObject {
    Q_OBJECT
public:
    // The object loads TLE data(from file or web) and emits calculated tracking data
    // We are tracking AO-27 at the moment! (satId is 22825)

    // Liberty Statue, Budapest is the default location for the station
    PredicterWorker(QThread *thread,
        QString dirString,
        long satID = 22825,
        double stationLat = 47.48675,
        double stationLon = -19.04804,
        int stationAlt = 275,
        QObject *parent = 0);

    void updateTLE();

    void setStation(double lat, double lon, int alt);

    // should emit data
    void trackSat();

    /*
    void printAOS();
    void printLOS();
    void printNextPassPath();
    */

private:
    QString dirString; //!< The folder that every file will be created in / opened from
    QString prefix;    //!< QString that stores the fileName prefix that will be prepended to the tracking data.
    bool preCalcNeeded_priv = true; //!< Indicates whether precalculation is necessary
    QTimer tracktimer_priv;  //!< QTimer object that calls \p PredicterWorker::trackSat() every second when tracking is
                             //!< enabled.
    QTimer updateTimer_priv; //!< QTimer object that calls \p PredicterWorker::updateTLESlot() every 12 hours.
    bool trackingDataSet_priv = false; //!< Indicates whether valid tracking data has been loaded.
    long satID_priv;                   //!< TLE ID of the satellite
    QNetworkAccessManager mgr_priv;    //!< QNetworkAccessManager that will download TLE data
    bool tracking_priv = false;        //!< Stores whether tracking is currently active
    bool loading_priv = false;         //!< Stores whether tracking is being started (downloading TLE, etc.)
    unsigned long baseFrequency_priv;  //!< The base frequency used for tracking

    void setTracking(bool newValue);
    void setLoading(bool newValue);
    QList<QByteArray> findDataInTLE(QString fileName);
    bool readTLEFromFile(QString fileName);
    const bool &tracking() const;
    const bool &loading() const;

    void startTracking();
    void TLEReady(QNetworkReply *reply);

signals:

    /**
     * @brief Emitted valid TLE data couldn't be acquired from either NORAD or manual.txt.
     */
    void tleErrorSignal();

    /**
     * @brief The signal that contains the calculated tracking data for the satellite. That data is utilized in QML.
     * @param azimuth Azimuth degrees.
     * @param elevation Azimuth degrees.
     * @param downlink_freq The calculated downlink frequency.
     * @param uplink_freq The calculated uplink frequency.
     * @param doppler100 The calculated doppler frequency at 100 MHz.
     * @param nextAOSQS The next acquisition of signal as a formatted QString.
     * @param nextLOSQS The next loss of signal as a formatted QString.
     */
    void trackingDataSignal(double azimuth,
        double elevation,
        double downlink_freq,
        double uplink_freq,
        double doppler100,
        QString nextAOSQS,
        QString nextLOSQS);

    /**
     * @brief Signal that is emitted when it has been detected that the satellite is never visible from the station.
     */
    void neverVisibleSignal();

    /**
     * @brief Signal that is emitted when it has been detected that the satellite appears to have decayed.
     */
    void decayedSignal();

    /**
     * @brief Signal that is emitted when tracking has stopped or resumed.
     */
    void trackingChanged(bool newValue);

    /**
     * @brief Signal that is emitted when tracking data is being loaded(TLE, etc.)
     */
    void loadingChanged(bool newValue);

    /**
     * @brief Signal that emits the next pass's azimuths as a QList<unsigned int>
     * @param nextPassAzimuths QList<unsigned int> of next pass's azimuths
     * @param nextPassTimestamp UTC timestamp for the next passing's start
     */
    void nextPassSignal(QList<unsigned int> nextPassAzimuths, long nextPassTimestamp);

public slots:
    void updateTLESlot();
    void changeStationSlot(double lat, double lon, int alt);
    void stopTrackingSlot();
    void requestNextPassSlot();
    void newBaseFrequency(unsigned long frequencyHz);
    void newSatID(long satID);
    void newPrefix(QString prefix);
};

#endif // PREDICTERWORKER_H
