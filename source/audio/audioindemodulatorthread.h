#ifndef AUDIOINDEMODULATORTHREAD_H
#define AUDIOINDEMODULATORTHREAD_H

#include "../packet/packetdecoder.h"
#include "audioindemodulator.h"
#include <QScopedPointer>
#include <QThread>

/**
 * @brief Class that inherits from QThread in order to achieve demodulation of audio samples on a different thread.
 */
class AudioInDemodulatorThread : public QThread {
    Q_OBJECT
private:
    QScopedPointer<AudioInDemodulator> demodulatorObject; //!< AudioInDemodulator instance that demodulates the samples.
public:
    AudioInDemodulatorThread(PacketDecoder *pd);
    ~AudioInDemodulatorThread();

public slots:
    void demodulate(std::int16_t *samples, int len);
    void changeSettings(unsigned int new_packet_length);

signals:
    /**
     * @brief Signal that emits to AudioInDemodulatorThread::demodulatorObject in order to have it demodulate audio
     * samples.
     * @param[in] samples The audio sampler data that will be processed. The samples wil have to be deleted after they
     * have been processed.
     * @param[in] len The number of samples that will be recived by AudioInDemodulatorThread::demodulatorObject.
     */
    void demodulateSignal(std::int16_t *samples, int len);

    /**
     * @brief Signal that emits to AudioInDemodulatorThread::demodulatorObject in order to change its packetsize.
     * @param[in] newPacketsize The new packet size that will be set.
     */
    void changeSettingsSignal(long newPacketsize);
};

#endif // AUDIOINDEMODULATORTHREAD_H
