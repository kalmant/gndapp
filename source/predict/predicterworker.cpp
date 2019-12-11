#include "predicterworker.h"

/**
 * @brief Constructor for the class.
 *
 * Initializes \p preCalcNeeded to true.
 * Sets the station's location to \p stationLat, \p stationLon and \p stationAlt.
 * The default station location is Liberty Statue, Budapest.
 * Sets up connections, initializes \p tracktimer_priv and \p updateTimer_priv and moves them to \p thread
 * Also moves \p mgr_priv to \p thread
 *
 * @param[in] dirString The folder that every will be created in / opened from
 * @param[in] stationLat The new latitude setting for the station. Negative latitude means South, positive is North.
 * @param[in] stationLon The new longitude setting for the station. Negative longitude means East, positive is West.
 * @param[in] stationAlt The new altitude setting for the station.
 * @param[in] parent The parent QObject, should be left empty.
 */
PredicterWorker::PredicterWorker(QThread *thread,
    QString dirString,
    long satID,
    double stationLat,
    double stationLon,
    int stationAlt,
    QObject *parent)
: QObject(parent) {
    Q_ASSERT(thread != nullptr);
    moveToThread(thread);
    tracktimer_priv.moveToThread(thread);
    updateTimer_priv.moveToThread(thread);
    mgr_priv.moveToThread(thread);
    this->dirString = dirString;
    this->prefix = "startup";
    preCalcNeeded_priv = true;
    setTracking(false);
    setLoading(false);
    setStation(stationLat, stationLon, stationAlt);
    this->satID_priv = satID;
    baseFrequency_priv = INITIALBASEFREQUENCY;

    connect(&mgr_priv, &QNetworkAccessManager::finished, this, [&](QNetworkReply *reply) { TLEReady(reply); });

    connect(&tracktimer_priv, &QTimer::timeout, [&]() { trackSat(); });

    connect(&updateTimer_priv, &QTimer::timeout, this, &PredicterWorker::updateTLESlot);
}

/**
 * @brief Invalidates previous data and redownloads or reloads TLE data.
 */
void PredicterWorker::updateTLE() {
    stopTrackingSlot();
    setLoading(true);
    trackingDataSet_priv = false;
    preCalcNeeded_priv = true;
    mgr_priv.get(QNetworkRequest(QUrl("https://www.celestrak.com/NORAD/elements/active.txt")));
}

/**
 * @brief Changes the location of the station to \p lat \p lon \p alt.
 * @param[in] lat The new latitude of the station.
 * @param[in] lon The new longitude of the station.
 * @param[in] alt The new altitude of the station.
 */
void PredicterWorker::setStation(double lat, double lon, int alt) {
    preCalcNeeded_priv = true;
    initQth(createQthStruct(lat, lon, alt));
}

/**
 * @brief Find the TLE data for the satellite in \p fileName.
 * @param[in] fileName The file to load TLE data from.
 * @return Returns a QList that has 2 elements when successful, returns an empty QList otherwise.
 */
QList<QByteArray> PredicterWorker::findDataInTLE(QString fileName) {
    QFile file(fileName);
    QList<QByteArray> ret;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        file.close();
        ret.clear();
        return ret;
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        // NORAD site replies with an html document if the name of the txt is incorrect
        if (line.startsWith("<!")) {
            ret.clear();
            return ret;
        }
        if (line.startsWith(QString("1 %1").arg(satID_priv).toLatin1())) {
            ret.append(line);
            line = file.readLine();
            if (line.startsWith(QString("2 %1").arg(satID_priv).toLatin1())) {
                ret.append(line);
                return ret;
            }
            else {
                ret.clear();
            }
        }
    }

    ret.clear();
    return ret;
}

// opens the file, checks whether the first character is <
// find the satellite data then processes it, closes the file and returns with the result
/**
 * @brief Initializes the tracking data from \p fileName.
 *
 * Calls PredicterWorker::findData() and loads the data.
 *
 * @param[in] fileName The name of the file to load TLE data from.
 * @return True if the load was successful, returns false otherwise.
 */
bool PredicterWorker::readTLEFromFile(QString fileName) {
    QList<QByteArray> ret = findDataInTLE(fileName);

    if (ret.length() == 2) {
        char line1[70];
        char line2[70];
        strncpy(line1, ret.at(0).data(), 69);
        strncpy(line2, ret.at(1).data(), 69);
        if (KepCheck(line1, line2) != 1) {
            return false;
        }
        else {
            initSat(line1, line2);
            trackingDataSet_priv = true;
            return true;
        }
    }
    else {
        return false;
    }
}

// The logic:
//    If the reply returns an error, loading manual.txt
//        If we find the two lines then initializes, else emits notle
//    Else we'll try to save the new file
//        In case we can't, we load manual.txt (if we find the two lines...)
//        If we can save it, we check whether it starts with '<' (NORAD replies with an HTML document if the filename
//        was incorrect)
//            If it does, then load manual, check it, etc...
//            If it doesn't, then it's presumably a normal TLE thus we check it , etc...
// We also create and check backups
/**
 * @brief Tries to acquire valid TLE data from either NORAD or manual.txt.
 * @param reply The QNetworkReply that hopefully contains the TLE data. Calls deleteLater() on it.
 */
void PredicterWorker::TLEReady(QNetworkReply *reply) {
    stopTrackingSlot();
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply_QSP(reply);
    QString manualFileName = dirString + "manual.txt";
    if (reply_QSP->error()) {
        if (readTLEFromFile(manualFileName)) {
            startTracking();
        }
        else {
            emit tleErrorSignal();
        }
    }
    else {
        QString tleName;
        tleName.append(dirString);
        tleName.append(QDateTime::currentDateTimeUtc().toString(QString("yyyyMMdd")));
        auto hour = QDateTime::currentDateTimeUtc().toString(QString("HH")).toInt();
        tleName.append(QString::number(hour/6 + 1));
        tleName.append(".txt");

        QFile f(tleName);
        if (f.open(QFile::WriteOnly | QFile::Text)) {
            QByteArray data = reply_QSP->readAll();
            f.write(data);
            f.flush();
            f.close();
            if (readTLEFromFile(tleName)) {
                startTracking();
                QFile f2(manualFileName);
                if (f2.open(QFile::WriteOnly | QFile::Text)) {
                    f2.write(data);
                    f2.flush();
                    f2.close();
                }
            }
            else {
                if (readTLEFromFile(manualFileName)) {
                    startTracking();
                }
                else {
                    emit tleErrorSignal();
                }
            }
        }
        else {
            if (readTLEFromFile(manualFileName)) {
                startTracking();
            }
            else {
                emit tleErrorSignal();
            }
        }
    }
}

/**
 * @brief If \p trackingDataSet_priv is true starts \p tracktimer_priv with a 1000 ms interval.
 */
void PredicterWorker::startTracking() {
    if (!trackingDataSet_priv) {
        stopTrackingSlot();
        return;
    }
    tracktimer_priv.start(1000);
    updateTimer_priv.start(21600000); // 6 hours
    setLoading(false);
    setTracking(true);
}

/**
 * @brief Stops \p tracktimer_priv and \p updateTimer_priv
 */
void PredicterWorker::stopTrackingSlot() {
    if (tracktimer_priv.isActive()) {
        tracktimer_priv.stop();
    }
    if (updateTimer_priv.isActive()) {
        updateTimer_priv.stop();
    }
    setLoading(false);
    setTracking(false);
}

/**
 * @brief Emits the data describing the next passing (emits \p PredicterWorker::nextPassSignal())
 */
void PredicterWorker::requestNextPassSlot() {
    emit nextPassSignal(predictNextPassPath(), nextAOS());
}

/**
 * @brief Changes the predict base frequency to \p frequencyHz
 * @param frequencyHz The new base frequency.
 */
void PredicterWorker::newBaseFrequency(unsigned long frequencyHz) {
    if (baseFrequency_priv != frequencyHz) {
        baseFrequency_priv = frequencyHz;
    }
}

/**
 * @brief Changes the satellite ID to \p satID
 * @param satID the new satellite ID
 */
void PredicterWorker::newSatID(long satID) {
    if (satID_priv != satID) {
        satID_priv = satID;
    }
}

/**
 * @brief The slot that prefix changes are connected to. Changes PredicterWorker::prefix to \p prefix
 * @param prefix The new prefix to use
 */
void PredicterWorker::newPrefix(QString prefix) {
    if (this->prefix != prefix) {
        this->prefix = prefix;
    }
}

/**
 * @brief Calls PredicterWorker::updateTLE().
 */
void PredicterWorker::updateTLESlot() {
    updateTLE();
}

/**
 * @brief Changes the location of the station to \p lat, \p lon, \p alt.
 *
 * Calls \p PredicterWorker::setStation().
 *
 * Resumes tracking if it was active.
 *
 * @param[in] lat New latitude for the station.
 * @param[in] lon New longitude for the station.
 * @param[in] alt New altitude for the station.
 */
void PredicterWorker::changeStationSlot(double lat, double lon, int alt) {
    bool wasRunning = tracktimer_priv.isActive();
    if (wasRunning)
        stopTrackingSlot();
    setStation(lat, lon, alt);
    if (wasRunning && trackingDataSet_priv)
        startTracking();
}

/**
 * @brief Handles tracking using track() from predictmod.
 *
 * Handles preCalc and logs the tracking data to trackingdata.txt.
 * The tracking data itself is emitted through Predicter::trackingDataSignal().
 */
void PredicterWorker::trackSat() {
    if (preCalcNeeded_priv) {
        int res = trackInit();
        switch (res) {
        case -2:
            // Satellite is never visible from the station
            emit neverVisibleSignal();
            stopTrackingSlot();
            return;
        case -1:
            // Satellite appears to have decayed
            emit decayedSignal();
            stopTrackingSlot();
            return;
        default:
            break;
        }
        preCalcNeeded_priv = false;
    }
    trackData result = track(baseFrequency_priv);
    emit trackingDataSignal(result.azimuth,
        result.elevation,
        result.downlink_freq,
        result.uplink_freq,
        result.doppler100,
        result.AOSTime,
        result.LOSTime);
    QFile f(dirString + prefix + "_" + "trackingdata.txt");
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f);
        out << "AZ: " << result.azimuth << "\nEL: " << result.elevation
            << "\nDL: " << static_cast<uint32_t>(result.downlink_freq)
            << "\nUL: " << static_cast<uint32_t>(result.uplink_freq) << "\nDoppler@100MHz: " << result.doppler100
            << "\nAOS: " << result.AOSTime << "\nLOS: " << result.LOSTime;
        out.flush();
        f.close();
    }
    else {
        return;
    }
}

void PredicterWorker::setTracking(bool newValue) {
    if (newValue != tracking_priv) {
        tracking_priv = newValue;
        emit trackingChanged(newValue);
    }
}

const bool &PredicterWorker::tracking() const {
    return tracking_priv;
}

void PredicterWorker::setLoading(bool newValue) {
    if (newValue != loading_priv) {
        loading_priv = newValue;
        emit loadingChanged(newValue);
    }
}

const bool &PredicterWorker::loading() const {
    return loading_priv;
}
