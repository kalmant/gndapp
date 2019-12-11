#ifndef AUDIOINDEMODULATOR_H
#define AUDIOINDEMODULATOR_H
#include <complex>
#include "../demod/newsmog1dem.h"
#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QScopedArrayPointer>
#include <math.h>


/**
 * @brief The class that 1250 BPS data received from an audio sampler.
 *
 * Demodulates the received data and emits the demodulated packet when it is found.
 *
 */
class AudioInDemodulator : public QObject {
    Q_OBJECT
private:
    void process_samples(std::int16_t *samples, int len);
    std::complex<float> resample_input_buffer[S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ];
    std::complex<float> resample_output_buffer[S1DEM_AUDIO_SAMPLING_FREQ];
    long packet_length_priv;
    long current_input_buffer_idx;
    std::complex<float> dem_a;
    std::complex<float> dem_b;
    bool dem_a_set;
    QVector<char> packet_characters;
    DEMVariables *variables_priv;                    //!< Variables pointer to the demodulation variables

public:
    AudioInDemodulator(long packet_length, QObject *parent = 0);
    ~AudioInDemodulator();
signals:
    /**
     * @brief Signal emitted when a new packet was demodulated.
     *
     * The signal is handled by a different object that receives this signal.
     *
     * @param[in] timestamp Timestamp for the packet.
     * @param source The source of the packet (in this case, it is audio)
     * @param[in] packetUpperHexString QString that represents the data contained in the packet as an UPPERCASE Hex
     * QString
     */
    void dataReady(QDateTime timestamp, QString source, QString packetUpperHexString);
public slots:
    void demodulateSlot(std::int16_t *samples, int len);
    void changeSettingsSlot(long new_packet_length);
};

#endif // AUDIOINDEMODULATOR_H
