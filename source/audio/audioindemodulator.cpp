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
        if (current_input_buffer_idx == S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ){
            rational_resample(resample_input_buffer, resample_output_buffer);
            for (int j = 0; j < S1DEM_AUDIO_SAMPLING_FREQ; j++){
                auto cncod = cnco(&variables_priv->cnco_vars, resample_output_buffer[j]);
                auto avgd = average(&variables_priv->avg_vars, cncod);
                std::complex<float> avg_dec_output;
                bool avg_dec_performed = average_dec(&variables_priv->avg_dec_vars, avgd, &avg_dec_output);
                if (avg_dec_performed){
                    auto demod_output = smog_atl_demodulate(&variables_priv->demod_vars, avg_dec_output, S1DEM_AUDIO_SAMPLING_FREQ, S1DEM_AUDIO_BPS);
                    if (dem_a_set){
                        dem_b = demod_output;
                        dem_a_set = false;
                        auto decd = make_hard_decision(&variables_priv->dec_vars, dem_a, dem_b, packet_length_priv);
                        if (decd != -1) {
                            packet_characters.append(char(decd));
                            if (packet_characters.length() == packet_length_priv) {
                                QDateTime timestamp = QDateTime::currentDateTimeUtc();
                                QString source = QString("Audio 1250 BPS");
                                QString packetUpperHexString = QString(QByteArray(reinterpret_cast<char *>(packet_characters.data()), packet_length_priv).toHex()).toUpper();
                                emit dataReady(timestamp, source, packetUpperHexString);
                                packet_characters.clear();
                            }
                        }
                    } else {
                        dem_a = demod_output;
                        dem_a_set = true;
                    }
                }
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
 * @param[in] packet_length The length of the packets.
 * @param[in] parent The parent QObject, should be left empty.
 */
AudioInDemodulator::AudioInDemodulator(long packet_length, QObject *parent) : QObject(parent) {
    packet_length_priv = packet_length;
    current_input_buffer_idx = 0;
    dem_a_set = false;
    packet_characters.clear();
    variables_priv = create_and_initialize_audio_variables();
}

AudioInDemodulator::~AudioInDemodulator() {
    free_dem_variables(variables_priv);
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
    packet_length_priv = new_packet_length;
}
