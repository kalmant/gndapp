#include "sdrworker.h"

/**
 * @brief Function that receives and demodulates the SDR data.
 *
 * This is a callback function called when there is available data from SDR.
 * It checks whether it should stop reading from SDR.
 * It makes sure that demodulation is executed with the currently valid shift (base + dynamic) frequency.
 * If multiple bitrates are enabled, it demodulates with every single one. --- NOT ANYMORE/YET
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
        if (*worker->ds_priv != worker->ds_freq) {
            worker->ds_freq = *worker->ds_priv;
            change_cnco_offset_frequency(&worker->cnco_vars, worker->baseOffset + worker->ds_freq);
        }
        worker->mutex_priv->unlock();
        for (int i = 0; i < (long) len; i += 2) {
            std::complex<float> input(static_cast<int8_t>(static_cast<uint8_t>(buf[i]) - 128),
                static_cast<int8_t>(static_cast<uint8_t>(buf[i + 1]) - 128)); // Removing DC
            auto cncod = cnco(&worker->cnco_vars, input);
            // 5x AVG_DEC
            auto avgd_5 = average(&worker->avg_vars_5, cncod);
            std::complex<float> avg_dec_output_5;
            bool avg_dec_5_performed = average_dec(&worker->avg_dec_vars_5, avgd_5, &avg_dec_output_5);
            if (avg_dec_5_performed) {
                // Parallel demodulation
                worker->magic_demod_1250.addSample(avg_dec_output_5);
                worker->magic_demod_2500.addSample(avg_dec_output_5);
                worker->magic_demod_5000.addSample(avg_dec_output_5);
                worker->magic_demod_12500.addSample(avg_dec_output_5);

                // 20x AVG_DEC
                auto avgd_20 = average(&worker->avg_vars_20, avg_dec_output_5);
                std::complex<float> avg_dec_output_20;
                bool avg_dec_20_performed = average_dec(&worker->avg_dec_vars_20, avgd_20, &avg_dec_output_20);
                if (avg_dec_20_performed) {
                    // Emit to spectogram
                    emit worker->complexSampleReady(avg_dec_output_20);
                }
            }
        }
    }
}

/**
 * @brief Constructor for the class.
 *
 * Initializes \p mutex_priv to \p mutex, \p canRun_priv to \p canRun, \p ds_priv to \p ds.
 * \p dev_index_priv is initialized as -1 to make sure automatic device detection is used.
 * \p dev_priv is set to nullptr, since there is no connection to any device yet.
 *
 * @param[in] mutex Pointer to the QMutex that makes multi-thread execution possible.
 * @param[in] canRun Pointer to the bool that lets us know when reading from the SDR should terminate.
 * @param[in] ds Pointer to the float that stores the current value of dynamic shift.
 * @param[in] pl Pointer to the float that stores the current value of packet length.
 * @param[in] dr Pointer to the long that stores the current value of datarate.
 * @param[in] parent Pointer to the parent QObject, should be left empty.
 */
SDRWorker::SDRWorker(QMutex *mutex, bool *canRun, int *ds, long *pl, long *dr, QObject *parent) : QObject(parent) {
    mutex_priv = mutex;
    canRun_priv = canRun;
    ds_priv = ds;
    packet_length_ptr = pl;
    datarate_ptr = dr;
    dev_index_priv = -1;
    dev_priv = nullptr;
    baseFrequency = INITIALBASEFREQUENCY;
    baseOffset = 0;

    // Setting up CNCO vars
    change_cnco_sampling_rate(&cnco_vars, 250000);

    // Setting up avg vars and avg dec vars for the first, 5x decimation
    reinitialize_avg_vars(&avg_vars_5, 10);
    reinitialize_avg_dec_vars(&avg_dec_vars_5, 5);

    // Setting up avg vars and avg dec vars for the second, 20x decimation
    reinitialize_avg_vars(&avg_vars_20, 40);
    reinitialize_avg_dec_vars(&avg_dec_vars_20, 20);

    QObject::connect(&magic_demod_1250, &MagicDemodulator::dataReady, this, &SDRWorker::dataReady);
    QObject::connect(&magic_demod_2500, &MagicDemodulator::dataReady, this, &SDRWorker::dataReady);
    QObject::connect(&magic_demod_2500, &MagicDemodulator::dataReady, this, &SDRWorker::dataReady);
    QObject::connect(&magic_demod_12500, &MagicDemodulator::dataReady, this, &SDRWorker::dataReady);
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
    free(cnco_vars.lo);
    free(avg_vars_5.buf);
    free(avg_dec_vars_5.buf);
    free(avg_vars_20.buf);
    free(avg_dec_vars_20.buf);
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
 * @return Returns false if there was a problem with the operation.
 */
bool SDRWorker::readFromSDR(int device_index, long samplesPerSecond, double ppm, int gain) {
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
    verbose_set_frequency(dev_priv, static_cast<uint32_t>(baseFrequency));
    verbose_set_sample_rate(dev_priv, static_cast<uint32_t>(samplesPerSecond));
    if (gain == 0) {
        verbose_auto_gain(dev_priv);
    }
    else {
        verbose_gain_set(dev_priv, nearest_gain(dev_priv, gain * 10));
    }
    verbose_ppm_set(dev_priv, ppm);

    verbose_reset_buffer(dev_priv);

    ds_freq = *ds_priv;

    // Resetting CNCO vars
    change_cnco_sampling_rate(&cnco_vars, 250000);
    // Setting initial cnco offset frequency
    change_cnco_offset_frequency(&cnco_vars, baseOffset + ds_freq);
    // Resetting AVG vars
    reinitialize_avg_vars(&avg_vars_5, 10);
    reinitialize_avg_dec_vars(&avg_dec_vars_5, 5);
    reinitialize_avg_vars(&avg_vars_20, 40);
    reinitialize_avg_dec_vars(&avg_dec_vars_20, 20);
    // Resetting MagicDemodulators
    magic_demod_1250.clear();
    magic_demod_2500.clear();
    magic_demod_5000.clear();
    magic_demod_12500.clear();

    *canRun_priv = true;

    r = rtlsdr_read_async(dev_priv, rtlsdr_callback, (void *) this, 0, buflen);
    if (r < 0) {
        qWarning() << "Error during SDR reading!";
        dev_priv = nullptr;
        emit SDRHasBeenDisconnected();
        return false;
    }
    if (dev_priv != nullptr) {
        rtlsdr_close(dev_priv);
        dev_priv = nullptr;
    }
    if (dev_priv != nullptr && 1 == has_device_been_lost(dev_priv)) {
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
 */
void SDRWorker::start(int device_index, long samplesPerSecond, double ppm, int gain) {
    bool success = readFromSDR(device_index, samplesPerSecond, ppm, gain);
    if (!success) {
        *canRun_priv = false;
    }
}

/**
 * @brief Slot, that stops reading from the device and deinitializes certain variables.
 */
void SDRWorker::stop() {
    mutex_priv->lock();
    if (*canRun_priv) {
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
void SDRWorker::newBaseFrequencies(unsigned long baseFrequency, long baseOffset) {
    if (this->baseFrequency != baseFrequency) {
        this->baseFrequency = baseFrequency;
    }
    if (this->baseOffset != baseOffset) {
        this->baseOffset = baseOffset;
    }
}
