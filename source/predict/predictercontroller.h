#ifndef PREDICTERCONTROLLER_H
#define PREDICTERCONTROLLER_H

#include "predicterworker.h"
#include <QObject>
#include <QScopedPointer>
#include <QThread>

/**
 * @brief A class that acts as a proxy between QML calls and the \p PredicterWorker object it hosts.
 *
 * It forwards calls from QML to \p PredicterWorker and vice versa.
 *
 */
class PredicterController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool tracking READ tracking NOTIFY trackingChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
private:
    bool tracking_priv = false; //!< Stores whether tracking is currently active
    bool loading_priv = false;  //!< Stores whether tracking is being started (downloading TLE, etc.)
    QThread workerThread_priv;  //!< Thread that \p predicter_priv is on
    QScopedPointer<PredicterWorker> predicter_priv; //!< Pointer to a \p PredicterWorker object that does the work

    void setTracking(bool newValue);
    void setLoading(bool newValue);
    const bool &tracking() const;
    const bool &loading() const;

public:
    explicit PredicterController(QString tleDirString, long satId, QObject *parent = nullptr);
    ~PredicterController();

    Q_INVOKABLE void stopTracking();
    Q_INVOKABLE void updateTLE();
    Q_INVOKABLE void changeStation(double lat, double lon, int alt);
    Q_INVOKABLE void changeSatID(long satID);

signals:
    void decayedSignal();
    void tleErrorSignal();
    void trackingDataSignal(double azimuth,
        double elevation,
        double downlink_freq,
        double uplink_freq,
        double doppler100,
        QString nextAOSQS,
        QString nextLOSQS);
    void neverVisibleSignal();
    void trackingChanged();
    void loadingChanged();
    void stopTrackingInPredicter();
    void updateTLEInPredicter();
    void changeStationInPredicter(double lat, double lon, int alt);
    void requestNextPassInPredicter();
    void nextPassSignal(QList<unsigned int> nextPassAzimuths, long nextPassTimestamp);
    void newBaseFrequencySignal(unsigned long frequencyHz);
    void changeSatIDInPredicter(long satID);
    void changePrefixInPredicter(QString prefix);
public slots:
    void trackingChangedFromPredicter(bool newValue);
    void loadingChangedFromPredicter(bool newValue);
    void decayedFromPredicter();
    void tleErrorFromPredicter();
    void requestNextPass();
    void trackingDataFromPredicter(double azimuth,
        double elevation,
        double downlink_freq,
        double uplink_freq,
        double doppler100,
        QString nextAOSQS,
        QString nextLOSQS);
    void neverVisibleFromPredicter();
    void nextPassFromPredicter(QList<unsigned int> nextPassAzimuths, long nextPassTimestamp);
    void newBaseFrequency(unsigned long frequencyHz);
    void changePrefix(QString prefix);
};

#endif // PREDICTERCONTROLLER_H
