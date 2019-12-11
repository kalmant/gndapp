#include "predictercontroller.h"

/**
 * @brief Instantiates a \p PredicterWorker with \p workerThread. Sets up connections to facilitate proxying and starts
 * \p thread.
 *
 * @param tleDirString the folder that TLE files will be created in / opened from
 * @param satId satellite ID of the satellite
 * @param parent The parent QObject, should be left empty.
 */
PredicterController::PredicterController(QString tleDirString, long satId, QObject *parent) : QObject(parent) {
    this->predicter_priv.reset(new PredicterWorker(&workerThread_priv, tleDirString, satId));
    connect(predicter_priv.data(),
        &PredicterWorker::trackingChanged,
        this,
        &PredicterController::trackingChangedFromPredicter);
    connect(predicter_priv.data(),
        &PredicterWorker::loadingChanged,
        this,
        &PredicterController::loadingChangedFromPredicter);
    connect(
        this, &PredicterController::stopTrackingInPredicter, predicter_priv.data(), &PredicterWorker::stopTrackingSlot);
    connect(this, &PredicterController::updateTLEInPredicter, predicter_priv.data(), &PredicterWorker::updateTLESlot);
    connect(this,
        &PredicterController::changeStationInPredicter,
        predicter_priv.data(),
        &PredicterWorker::changeStationSlot);
    connect(predicter_priv.data(), &PredicterWorker::tleErrorSignal, this, &PredicterController::tleErrorFromPredicter);
    connect(predicter_priv.data(),
        &PredicterWorker::neverVisibleSignal,
        this,
        &PredicterController::neverVisibleFromPredicter);
    connect(predicter_priv.data(), &PredicterWorker::decayedSignal, this, &PredicterController::decayedFromPredicter);
    connect(predicter_priv.data(),
        &PredicterWorker::trackingDataSignal,
        this,
        &PredicterController::trackingDataFromPredicter);
    connect(this,
        &PredicterController::requestNextPassInPredicter,
        predicter_priv.data(),
        &PredicterWorker::requestNextPassSlot);
    connect(predicter_priv.data(), &PredicterWorker::nextPassSignal, this, &PredicterController::nextPassFromPredicter);
    connect(
        this, &PredicterController::newBaseFrequencySignal, predicter_priv.data(), &PredicterWorker::newBaseFrequency);
    connect(this, &PredicterController::changeSatIDInPredicter, predicter_priv.data(), &PredicterWorker::newSatID);
    connect(this, &PredicterController::changePrefixInPredicter, predicter_priv.data(), &PredicterWorker::newPrefix);
    workerThread_priv.start();
    setTracking(false);
    setLoading(false);
}

PredicterController::~PredicterController() {
    emit stopTrackingInPredicter();
    workerThread_priv.wait(10); // waiting for a short while until the timers stop
    workerThread_priv.quit();
    workerThread_priv.wait();
}

/**
 * @brief Tells \p predicter_priv to stop tracking
 */
void PredicterController::stopTracking() {
    emit stopTrackingInPredicter();
}

/**
 * @brief Tells \p predicter_priv to update the TLE data
 */
void PredicterController::updateTLE() {
    emit updateTLEInPredicter();
}

/**
 * @brief Tells \p predicter_priv to change the station location to \p lat \p lon \p alt.
 * @param[in] lat The new latitude of the station.
 * @param[in] lon The new longitude of the station.
 * @param[in] alt The new altitude of the station.
 */
void PredicterController::changeStation(double lat, double lon, int alt) {
    emit changeStationInPredicter(lat, lon, alt);
}

/**
 * @brief Tells \p predicter_priv to change the satID to \p satID
 * @param satID The new satellite ID
 */
void PredicterController::changeSatID(long satID) {
    emit changeSatIDInPredicter(satID);
}

/**
 * @brief Requests the next passing from \p predicter_priv.
 */
void PredicterController::requestNextPass() {
    emit requestNextPassInPredicter();
}

/**
 * @brief Reflects the change in \p predicter_priv 's current tracking state
 * @param newValue The current tracking state
 */
void PredicterController::trackingChangedFromPredicter(bool newValue) {
    setTracking(newValue);
}

/**
 * @brief Reflects the change in \p predicter_priv 's current loading state
 * @param newValue The current loading state
 */
void PredicterController::loadingChangedFromPredicter(bool newValue) {
    setLoading(newValue);
}

/**
 * @brief Relays \p PredicterWorker::decayedSignal() from \p predicter_priv.
 */
void PredicterController::decayedFromPredicter() {
    emit decayedSignal();
}

/**
 * @brief Relays \p PredicterWorker::tleErrorSignal() from \p predicter_priv.
 */
void PredicterController::tleErrorFromPredicter() {
    emit tleErrorSignal();
}

/**
 * @brief Relays \p PredicterWorker::trackingDataSignal() from \p predicter_priv.
 * @param azimuth Azimuth degrees.
 * @param elevation Azimuth degrees.
 * @param downlink_freq The calculated downlink frequency.
 * @param uplink_freq The calculated uplink frequency.
 * @param doppler100 The calculated doppler frequency at 100 MHz.
 * @param nextAOSQS The next acquisition of signal as a formatted QString.
 * @param nextLOSQS The next loss of signal as a formatted QString.
 */
void PredicterController::trackingDataFromPredicter(double azimuth,
    double elevation,
    double downlink_freq,
    double uplink_freq,
    double doppler100,
    QString nextAOSQS,
    QString nextLOSQS) {
    emit trackingDataSignal(azimuth, elevation, downlink_freq, uplink_freq, doppler100, nextAOSQS, nextLOSQS);
}

/**
 * @brief Relays \p PredicterWorker::neverVisibleSignal() from \p predicter_priv.
 */
void PredicterController::neverVisibleFromPredicter() {
    emit neverVisibleSignal();
}

/**
 * @brief Relays \p PredicterWorker::nextPassSignal() from \p predicter_priv.
 * @param nextPassAzimuths The QList<unsigned int> of azimuths.
 * @param nextPassTimestamp UTC timestamp for the next passing's start
 */
void PredicterController::nextPassFromPredicter(QList<unsigned int> nextPassAzimuths, long nextPassTimestamp) {
    emit nextPassSignal(nextPassAzimuths, nextPassTimestamp);
}

void PredicterController::newBaseFrequency(unsigned long frequencyHz) {
    emit newBaseFrequencySignal(frequencyHz);
}

/**
 * @brief The slot that prefix changes are connected to. Signals to \p predicter_priv to change the prefix.
 * @param prefix The new prefix to use
 */
void PredicterController::changePrefix(QString prefix) {
    emit changePrefixInPredicter(prefix);
}

void PredicterController::setTracking(bool newValue) {
    if (newValue != tracking_priv) {
        tracking_priv = newValue;
        emit trackingChanged();
    }
}

const bool &PredicterController::tracking() const {
    return tracking_priv;
}

void PredicterController::setLoading(bool newValue) {
    if (newValue != loading_priv) {
        loading_priv = newValue;
        emit loadingChanged();
    }
}

const bool &PredicterController::loading() const {
    return loading_priv;
}
