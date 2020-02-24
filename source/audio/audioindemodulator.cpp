#include "audioindemodulator.h"

/**
 * @brief Processes \p len number of \p samples. Emits AudioInDemodulator::dataReady() if a packet was found.
 *
 * Iterates over \p samples and processes them.
 * When a packet was found a properly formatted QString is created.
 * Then that QString is emitted through AudioInDemodulator::dataReady().
 * Takes ownership of \p samples.
 *
 * @param[in] samples The samples from audiosampler. The method takes ownership of it.
 * @param[in] len The number of samples.
 */
void AudioInDemodulator::process_samples(int16_t *samples, int len) {
    QScopedArrayPointer<int16_t> samples_qsap(samples);
    for (int i = 0; i < len; i++) {
        resample_input_buffer[current_input_buffer_idx] = s16le2cf(samples[i]);
        current_input_buffer_idx++;
        if (current_input_buffer_idx == S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ) {
            rational_resample(resample_input_buffer, resample_output_buffer);
            for (int j = 0; j < S1DEM_AUDIO_SAMPLING_FREQ; j++) {
                auto cncod = cnco(&cnco_vars, resample_output_buffer[j]);
                magic_demod.addSample(cncod);
            }
            current_input_buffer_idx = 0;
        }
    }
}

/**
 * @brief Constructor for the class.
 *
 * Initializes the object. Sets packet length to \p packet_length.
 * Initializes a number of private variables.
 * @param[in] parent The parent QObject, should be left empty.
 */
AudioInDemodulator::AudioInDemodulator(QObject *parent) : QObject(parent) {
    current_input_buffer_idx = 0;
    change_cnco_sampling_rate(&cnco_vars, S1DEM_AUDIO_SAMPLING_FREQ); // After resampling, sampling rate is 42.5 kHz
    change_cnco_offset_frequency(&cnco_vars, S1DEM_AUDIO_FREQ_CENTER_OFFSET); // We have an offset of 1.5 kHz
    // Forwarding the signal
    QObject::connect(&magic_demod, &MagicDemodulator::dataReady, this, &AudioInDemodulator::dataReady);
}

AudioInDemodulator::~AudioInDemodulator() {
    free(cnco_vars.lo);
}

/**
 * @brief Slot used to call AudioInDemodulator::process_samples() from another thread.
 * @param[in] samples The samples from the audio sampler. Takes ownership of the pointer.
 * @param[in] len The number of samples forwarded for demodulation.
 */
void AudioInDemodulator::demodulateSlot(int16_t *samples, int len) {
    process_samples(samples, len);
}

/**
 * @brief Slot used to change certain the size of packets from another thread.
 * @param[in] new_packet_length New length of the packets.
 */
void AudioInDemodulator::changeSettingsSlot(long new_packet_length) {
    (void) new_packet_length;
    // NOTE: packet_length is a thing of the past since we now demodulate with every packet length
}

void AudioInDemodulator::resetDemodulatorSlot() {
    magic_demod.clear();
}
