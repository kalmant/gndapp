#include "audioindemodulatorthread.h"
#include "audiosampler.h"

/**
 * @brief Constructor for the class. Creates a new AudioInDemodulator with the specified \p datarate and sets up the
 * necessary connections.
 * @param[in] ptm Pointer to a \p PacketDecoder instance that receives the emitted demodulated packets.
 */
AudioInDemodulatorThread::AudioInDemodulatorThread(PacketDecoder *pd) {
    demodulatorObject.reset(new AudioInDemodulator(64));
    demodulatorObject->moveToThread(this); // may be unnecessary because the object itself was created on this thread
    QObject::connect(
        demodulatorObject.data(), &AudioInDemodulator::dataReady, pd, &PacketDecoder::decodablePacketReceived);
    QObject::connect(this,
        &AudioInDemodulatorThread::demodulateSignal,
        demodulatorObject.data(),
        &AudioInDemodulator::demodulateSlot,
        Qt::QueuedConnection);
    QObject::connect(this,
        &AudioInDemodulatorThread::changeSettingsSignal,
        demodulatorObject.data(),
        &AudioInDemodulator::changeSettingsSlot,
        Qt::QueuedConnection);
    QObject::connect(pd, &PacketDecoder::newPacketLength, this, &AudioInDemodulatorThread::changeSettings);
}

/**
 * @brief Ensures that the QThread is properly terminated
 */
AudioInDemodulatorThread::~AudioInDemodulatorThread() {
    this->quit();
    this->wait();
}

/**
 * @brief Slot that emits AudioInDemodulatorThread::demodulateSignal(), that is received by
 * AudioInDemodulator::demodulatorObject.
 * @param[in] samples The samples that are forwarded to the demodulator. Passes ownership of the pointer.
 * @param[in] len The number of samples that are forwarded to the demodulator.
 */
void AudioInDemodulatorThread::demodulate(int16_t *samples, int len) {
    emit demodulateSignal(samples, len);
}

/**
 * @brief Slot that emits a properly parametrized AudioInDemodulatorThread::changeSettingsSignal().
 * @param[in] new_packet_length The new packet length for the demodulator.
 */
void AudioInDemodulatorThread::changeSettings(unsigned int new_packet_length) {
    emit changeSettingsSignal(new_packet_length);
}
