#include "sdrthread.h"

/**
 * @brief Constructor for the class.
 *
 * Initializes \p df_priv, \p mut_priv, \p canRun_priv and \p sdrWorker.
 * Sets up the necessary connections with \p pd and \p predicter
 *
 * @param[in] pd Pointer to the \p PacketDecoder that receives the demodulated packets
 * @param[in] predicter Pointer to the \p PredicterController that sends tracking data
 */
SDRThread::SDRThread(PacketDecoder *pd, PredicterController *predicter) : QThread() {
    df_priv = 0;
    offset_priv = 0;
    dynamic_shift_priv = 0;
    ds_priv.reset(new int(0));
    mut_priv.reset(new QMutex());
    canRun_priv.reset(new bool(false));
    pl_priv.reset(new long(70));
    dr_priv.reset(new long(1250));
    canRun_mirror_priv = false;
    baseFrequency_priv = INITIALBASEFREQUENCY;
    baseOffset_priv = 0;
    dataRateBPS_priv = 1250;
    packetLengthBytes_priv = 70;
    sdrWorker.reset(new SDRWorker(mut_priv.data(), canRun_priv.data(), ds_priv.data(), pl_priv.data(), dr_priv.data()));
    QObject::connect(sdrWorker.data(), &SDRWorker::dataReady, this, &SDRThread::decodablePacketReceivedSlot);
    QObject::connect(this, &SDRThread::decodablePacketReceivedSignal, pd, &PacketDecoder::decodablePacketReceived);
    sdrWorker->moveToThread(this); // may be unnecessary because the object itself was created on this thread
    QObject::connect(predicter, &PredicterController::trackingDataSignal, this, &SDRThread::trackingDataSlot);

    QObject::connect(pd, &PacketDecoder::newDataRate, this, &SDRThread::newDataRateSlot);
    QObject::connect(pd, &PacketDecoder::newPacketLength, this, &SDRThread::newPacketLengthSlot);

    QObject::connect(this, &SDRThread::startSignal, sdrWorker.data(), &SDRWorker::start);
    QObject::connect(this, &SDRThread::stopSignal, sdrWorker.data(), &SDRWorker::stop);
    // The worker sends this if the SDR couldn't be started
    QObject::connect(sdrWorker.data(), &SDRWorker::cannotConnectToSDR, this, &SDRThread::cannotConnectToSDRSlot);
    // The worker sends this if the SDR has been disconnected during reading
    QObject::connect(
        sdrWorker.data(), &SDRWorker::SDRHasBeenDisconnected, this, &SDRThread::SDRHasBeenDisconnectedSlot);
    QObject::connect(sdrWorker.data(), &SDRWorker::SDRWasntStarted, this, &SDRThread::SDRWasntStartedSlot);
    // base frequency change
    QObject::connect(this, &SDRThread::newBaseFrequencies, sdrWorker.data(), &SDRWorker::newBaseFrequencies);

    // Forwarding complex sample for spectogram
    QObject::connect(sdrWorker.data(), &SDRWorker::complexSampleReady, this, &SDRThread::complexSampleReady);

    QTimer::singleShot(150, [&]() { refreshSdrDevices(); });
}

/**
 * @brief Ensures that the QThread is properly terminated
 */
SDRThread::~SDRThread() {
    terminateWorker();
    this->quit();
    this->wait();
}

/**
 * @brief Starts reading the SDR.
 *
 * Sets \p canRun_priv to true.
 * Emits a properly parametrized SDRThread::startSignal().
 *
 * @param[in] device_index The device's index
 * @param[in] ppm PPM error for the SDR.
 * @param[in] gain Gain for the SDR.
 * @param[in] automaticDF True if doppler frequency is automatically controlled (through tracking)
 */
void SDRThread::startReading(int device_index, double ppm, int gain, bool automaticDF) {
    if (device_index >= sdrDevicesModel.rowCount()) {
        emit invalidSdrDeviceIndex();
        return;
    }
    automaticDF_priv = automaticDF;
    emit startSignal(device_index, 250000, ppm, gain);
    canRun_mirror_priv = true;
}

/**
 * @brief Stops reading from SDR.
 *
 * Locks \p mut_priv and stops the worker.
 * Emits SDRTHread::stopSignal() after unlocking \p mut_priv.
 */
void SDRThread::stopReading() {
    mut_priv.data()->lock();
    *(canRun_priv.data()) = false;
    canRun_mirror_priv = false;
    mut_priv.data()->unlock();
    emit stopSignal();
}

/**
 * @brief Set manual offset
 * @param offset manual offset specified in Hz
 */
void SDRThread::setOffset(long offset) {
    if (offset_priv != offset) {
        offset_priv = offset;
        refreshDynamicShiftFrequency();
    }
}

/**
 * @brief Sets dynamic shift frequency if necessary. When a change is made the worker's ds is changed.
 */
void SDRThread::refreshDynamicShiftFrequency() {
    if (dynamic_shift_priv != offset_priv + df_priv) {
        dynamic_shift_priv = offset_priv + df_priv;
        mut_priv.data()->lock();
        *(ds_priv.data()) = dynamic_shift_priv;
        mut_priv.data()->unlock();
        emit currentFrequencyChanged();
    }
}

void SDRThread::terminateWorker() {
    *(canRun_priv.data()) = false;
}

/**
 * @brief Sets the currently used doppler frequency.
 *
 * @param[in] newDF New value for the doppler frequency.
 */
void SDRThread::setDopplerFrequency(int newDF) {
    if (newDF != df_priv) {
        df_priv = newDF;
        refreshDynamicShiftFrequency();
    }
}

void SDRThread::refreshSdrDevices() {
    qInfo() << "Updating SDR devices";
    QStringList ret;
    auto device_count = custom_get_device_count();
    qInfo() << device_count << "SDR devices found";
    for (uint32_t i = 0; i < device_count; i++) {
        ret.append(QString(custom_get_device_name(i)));
    }
    sdrDevicesModel.setStringList(ret);
    emit sdrDevicesChanged();
}

QStringListModel *SDRThread::sdrDevices() {
    return &sdrDevicesModel;
}

long SDRThread::currentFrequency() {
    return baseFrequency_priv + baseOffset_priv + dynamic_shift_priv;
}

/**
 * @brief The slot receives a signal when the SDR couldn't be started.
 *
 * Emits SDRThread::cannotConnectToSDRSignal().
 */
void SDRThread::cannotConnectToSDRSlot() {
    canRun_mirror_priv = false;
    emit cannotConnectToSDRSignal();
}

/**
 * @brief The slot receives a signal when the SDR has been disconnected during an operation.
 *
 * Emits SDRThread::sdrDisconnected().
 */
void SDRThread::SDRHasBeenDisconnectedSlot() {
    canRun_mirror_priv = false;
    emit sdrDisconnected();
}

/**
 * @brief The slot receives a signal when the SDR was not started due to memory allocation issues
 */
void SDRThread::SDRWasntStartedSlot() {
    canRun_mirror_priv = false;
    emit cannotConnectToSDRSignal(); // Lazy solution, no separate signal is emitted for this
}

/**
 * @brief Performs the necessary actions upon receiving tracking data.
 *
 * Sets doppler frequency to \p doppler100 * (baseFrequency_priv+baseOffset)/100000000
 *
 * @param azimuth UNUSED
 * @param elevation UNUSED
 * @param downlink_freq UNUSED
 * @param uplink_freq UNUSED
 * @param doppler100 Doppler frequency for 100MHz. We are operating on baseFrequency_priv
 * @param nextAOSQS UNUSED
 * @param nextLOSQS UNUSED
 */
void SDRThread::trackingDataSlot(double azimuth,
    double elevation,
    double downlink_freq,
    double uplink_freq,
    double doppler100,
    QString nextAOSQS,
    QString nextLOSQS) {
    (void) azimuth;
    (void) elevation;
    (void) downlink_freq;
    (void) uplink_freq;
    (void) nextAOSQS;
    (void) nextLOSQS;

    if (automaticDF_priv) {
        setDopplerFrequency(
            static_cast<int>(doppler100 * (static_cast<double>(baseFrequency_priv + baseOffset_priv) / 100000000)));
    }
}

/**
 * @brief Slot that receives the new base frequency when it should be changed
 * @param frequencyHz new base frequency
 */
void SDRThread::newBaseFrequenciesSlot(unsigned long baseFrequency, long baseOffset) {
    baseFrequency_priv = baseFrequency;
    baseOffset_priv = baseOffset;
    emit newBaseFrequencies(baseFrequency_priv, baseOffset_priv);
    emit currentFrequencyChanged();
}

/**
 * @brief Slot that receives new datarate signals from a PacketDecoder.
 *
 * Restarts SDRWorker with the change if it is necessary.
 *
 * @param newDataRateBPS New datarate [BPS]
 */
void SDRThread::newDataRateSlot(unsigned int newDataRateBPS) {
    if (newDataRateBPS != dataRateBPS_priv) {
        dataRateBPS_priv = newDataRateBPS;
        mut_priv.data()->lock();
        *(dr_priv.data()) = dataRateBPS_priv;
        mut_priv.data()->unlock();
    }
}

/**
 * @brief Slot that receives new packet length signals from a PacketDecoder.
 *
 * Restarts SDRWorker with the change if it is necessary.
 *
 * @param newPacketLengthBytes New packet length [bytes]
 */
void SDRThread::newPacketLengthSlot(unsigned int newPacketLengthBytes) {
    if (newPacketLengthBytes != packetLengthBytes_priv) {
        packetLengthBytes_priv = newPacketLengthBytes;
        mut_priv.data()->lock();
        *(pl_priv.data()) = packetLengthBytes_priv;
        mut_priv.data()->unlock();
    }
}

/**
 * @brief Slot for new packets from the SDRWorker
 * @param timestamp UTC timestamp
 * @param source Source of the packet (SDR)
 * @param packetUpperHexString The packet data as an upper hex string
 */
void SDRThread::decodablePacketReceivedSlot(QDateTime timestamp, QString source, QString packetUpperHexString) {
    emit decodablePacketReceivedSignal(timestamp, source, packetUpperHexString);
}
