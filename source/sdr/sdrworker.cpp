#include "sdrworker.h"

/**
 * @brief Function that receives and demodulates the SDR data.
 *
 * This is a callback function called when there is available data from SDR.
 * It checks whether it should stop reading from SDR.
 * It makes sure that demodulation is executed with the currently valid doppler frequency.
 * If multiple bitrates are enabled, it demodulates with every single one.
 *
 * @param[in] buf Array of unsigned characters, that are received from SDR.
 * @param[in] len Twice the number of characters received. This is because the data is stored as 2 bytes.
 * @param[in] ctx Pointer to the context that lets us access SDRWorker from a global function.
 */
void rtlsdr_callback(unsigned char *buf, unsigned int len, void *ctx) {
    if (ctx) {
        SDRWorker *worker = reinterpret_cast<SDRWorker *>(ctx);
        worker->mutex_priv->lock();
        if (!*(worker->canRun_priv)) {
            rtlsdr_cancel_async(worker->dev_priv);
            worker->mutex_priv->unlock();
            return;
        }
        if (*worker->df_priv != worker->doppler_freq){
            worker->doppler_freq = *worker->df_priv;
            SDR_set_doppler_frequency(worker->vars, worker->doppler_freq);
        }
        worker->packet_length = *worker->packet_length_ptr;
        if (*worker->datarate_ptr != worker->datarate){
            worker->datarate = *worker->datarate_ptr;
            SDR_set_datarate(worker->vars, worker->datarate);
        }
        worker->mutex_priv->unlock();
        for (int i = 0; i < (long) len; i += 2) {
            std::complex<float> input(static_cast<int8_t>(static_cast<uint8_t>(buf[i])-128), static_cast<int8_t>(static_cast<uint8_t>(buf[i+1])-128)); // Removing DC
            auto cncod = cnco(&worker->vars->cnco_vars, input);
            auto avgd = average(&worker->vars->avg_vars, cncod);
            std::complex<float> avg_dec_output;
            bool avg_dec_performed = average_dec(&worker->vars->avg_dec_vars, avgd, &avg_dec_output);
            if (avg_dec_performed){
                auto demod_output = smog_atl_demodulate(&worker->vars->demod_vars, avg_dec_output, S1DEM_SDR_SAMPLING_FREQ, worker->datarate);
                if (worker->dem_a_set){
                    worker->dem_b = demod_output;
                    worker->dem_a_set = false;
                    auto decd = make_hard_decision(&worker->vars->dec_vars, worker->dem_a, worker->dem_b, worker->packet_length);
                    if (decd != -1) {
                        worker->packet_characters.append(char(decd));
                        if (worker->packet_characters.length() == worker->packet_length) {
                            QDateTime timestamp = QDateTime::currentDateTimeUtc();
                            QString source = QString("SDR %1 BPS").arg(worker->datarate);
                            QString packetUpperHexString = QString(QByteArray(reinterpret_cast<char *>(worker->packet_characters.data()), worker->packet_length).toHex()).toUpper();
                            emit worker->dataReady(timestamp, source, packetUpperHexString);
                            worker->packet_characters.clear();
                        }
                    }
                } else {
                    worker->dem_a = demod_output;
                    worker->dem_a_set = true;
                }
            }
        }
    }
}

/**
 * @brief Constructor for the class.
 *
 * Initializes \p mutex_priv to \p mutex, \p canRun_priv to \p canRun, \p df_priv to \p priv.
 * \p dev_index_priv is initialized as -1 to make sure automatic device detection is used.
 * \p dev_priv is set to nullptr, since there is no connection to any device yet.
 *
 * @param[in] mutex Pointer to the QMutex that makes multi-thread execution possible.
 * @param[in] canRun Pointer to the bool that lets us know when reading from the SDR should terminate.
 * @param[in] df Pointer to the float that stores the current value of doppler frequency.
 * @param[in] pl Pointer to the float that stores the current value of packet length.
 * @param[in] dr Pointer to the long that stores the current value of datarate.
 * @param[in] parent Pointer to the parent QObject, should be left empty.
 */
SDRWorker::SDRWorker(QMutex *mutex, bool *canRun, int *df, long* pl, long* dr, QObject *parent) : QObject(parent) {
    mutex_priv = mutex;
    canRun_priv = canRun;
    df_priv = df;
    packet_length_ptr = pl;
    datarate_ptr = dr;
    dev_index_priv = -1;
    dev_priv = nullptr;
    baseFrequency = INITIALBASEFREQUENCY;
    vars = nullptr;
}

/**
 * @brief Calls stop to free any remaining resources.
 */
SDRWorker::~SDRWorker() {
    this->terminate();
    cleanup();
}

/**
 * @brief Frees resources.
 */
void SDRWorker::cleanup() {
    if (vars != nullptr) {
        free_dem_variables(vars);
    }
}

/**
 * @brief Initiates the asynchronous reading from SDR.
 *
 * Sets up the necessary parameters, finds the device and handles some exceptions.
 *
 * @param[in] device_index Device index of the SDR.
 * @param[in] samplesPerSecond Samping rate for SDR.
 * @param[in] ppm PPM error for the SDR.
 * @param[in] gain Gain for the SDR.
 * @param[in] dataRate Data rate of the demodulation.
 * @param[in] df Doppler frequency to start the demodulation with.
 * @param[in] packetLength Length of the packets.
 * @return Returns false if there was a problem with the operation.
 */
bool SDRWorker::readFromSDR(int device_index,
    long samplesPerSecond,
    int ppm,
    int gain,
    unsigned int dataRate,
    int offset,
    float df,
    unsigned int packetLength) {
    Q_UNUSED(dataRate);
    Q_UNUSED(df);
    Q_UNUSED(packetLength);
    int r;
    uint32_t buflen = 16 * 16384;
    if (device_index < 0) {
        const char *zero = "0";
        dev_index_priv = verbose_device_search(zero);
        if (dev_index_priv < 0) {
            // Couldn't connect to SDR, presumably no device is connected
            emit cannotConnectToSDR();
            qWarning() << "Cannot connect to SDR";
            return false;
        }
    }
    else {
        dev_index_priv = device_index;
    }
    r = rtlsdr_open(&dev_priv, (uint32_t) dev_index_priv);
    if (r < 0) {
        emit cannotConnectToSDR();
        return false;
    }
    verbose_set_frequency(dev_priv, static_cast<uint32_t>(baseFrequency + static_cast<unsigned long>(offset)));
    verbose_set_sample_rate(dev_priv, static_cast<uint32_t>(samplesPerSecond));
    if (gain == 0) {
        verbose_auto_gain(dev_priv);
    }
    else {
        verbose_gain_set(dev_priv, nearest_gain(dev_priv, gain * 10));
    }
    verbose_ppm_set(dev_priv, ppm);

    verbose_reset_buffer(dev_priv);

    // Deleting the variables struct and data
    cleanup();

    datarate = *datarate_ptr;
    doppler_freq = *df_priv;
    packet_length = *packet_length_ptr;
    vars = create_and_initialize_sdr_variables(doppler_freq, datarate);

    *canRun_priv = true;

    r = rtlsdr_read_async(dev_priv, rtlsdr_callback, (void *) this, 0, buflen);
    if (r < 0) {
        qWarning() << "Error during SDR reading!";
        dev_priv = nullptr;
        emit SDRHasBeenDisconnected();
        return false;
    }
    if (dev_priv != nullptr){
        rtlsdr_close(dev_priv);
        dev_priv = nullptr;
    }
    if (dev_priv !=nullptr && 1 == has_device_been_lost(dev_priv)) {
        // The device has been lost for some reason (has been disconnected)
        qWarning() << "SDR Device has been lost!";
        dev_priv = nullptr;
        emit SDRHasBeenDisconnected();
        return false;
    }
    return true;
}

/**
 * @brief Slot that calls SDRWorker::readFromSDR.
 * @param[in] device_index Device index of the SDR.
 * @param[in] samplesPerSecond Samping rate for SDR.
 * @param[in] ppm PPM error for the SDR.
 * @param[in] gain Gain for the SDR.
 * @param[in] dataRate Data rate of the demodulation.
 * @param[in] offset Offset frequency for the SDR.
 * @param[in] df Doppler frequency to start the demodulation with.
 * @param[in] packetLength Length of the packets.
 */
void SDRWorker::start(int device_index,
    long samplesPerSecond,
    int ppm,
    int gain,
    unsigned int dataRate,
    int offset,
    float df,
    unsigned int packetLength) {
    bool success = readFromSDR(device_index, samplesPerSecond, ppm, gain, dataRate, offset, df, packetLength);
    if (!success) {
        *canRun_priv = false;
    }
}

/**
 * @brief Slot, that stops reading from the device and deinitializes certain variables.
 */
void SDRWorker::stop() {
    mutex_priv->lock();
    if (*canRun_priv){
        qWarning() << "SDR stop was called while the device is still allowed to run";
    }
    else if (dev_priv != nullptr) {
        rtlsdr_cancel_async(dev_priv);
        rtlsdr_close(dev_priv);
        dev_priv = nullptr;
    }
    mutex_priv->unlock();
}

void SDRWorker::terminate() {
    if (dev_priv != nullptr) {
        rtlsdr_cancel_async(dev_priv);
        rtlsdr_close(dev_priv);
    }
}

/**
 * @brief Slot that is used to change the base frequency
 * @param frequencyHz new base frequency
 */
void SDRWorker::newBaseFrequency(unsigned long frequencyHz) {
    if (baseFrequency != frequencyHz) {
        baseFrequency = frequencyHz;
    }
}
