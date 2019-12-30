#ifndef SDRWORKER_H
#define SDRWORKER_H

#include "../demod/newsmog1dem.h"
#include "convenience.h"
#include "rtl-sdr.h"
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QObject>
#include <QScopedArrayPointer>
#include <time.h>

/**
 * @brief The SDRWorker class
 *
 * Maintains connection with the SDR and handles reading from the device.
 */
class SDRWorker : public QObject {
    Q_OBJECT
private:
    int dev_index_priv; //!< SDR device index


    void cleanup();

public:
    QMutex *mutex_priv;     //!< Pointer to the QMmutex that is used to handle multi-thread execution.
    bool *canRun_priv;      //!< Pointer to the bool that indicates whether the reading should stop.
    int *df_priv;           //!< Pointer to the integer that lets SDRWorker know how much the doppler frequency is.
    rtlsdr_dev_t *dev_priv; //!< Pointer to the currently used RTLSDR device.
    int doppler_freq;

    unsigned long baseFrequency;
    long packet_length;
    long *packet_length_ptr;
    long datarate;
    long *datarate_ptr;
    std::complex<float> dem_a;
    std::complex<float> dem_b;
    bool dem_a_set;
    QVector<char> packet_characters;


    DEMVariables* vars; //!< Pointer to a  Variables, which store the necessary data for SDR demodulation

    explicit SDRWorker(QMutex *mutex, bool *canRun, int *df, long* pl, long *dr, QObject *parent = 0);
    ~SDRWorker();

    bool readFromSDR(int device_index,
        long samplesPerSecond,
        int ppm,
        int gain,
        unsigned int dataRate,
        int offset,
        float df,
        unsigned int packetLength);

signals:
    /**
     * @brief Signal that is emitted when a packet has been detected. SDRThread forwards it to a PacketDecoder.
     * @param[in] timestamp QString that represents the current time in UTC.
     * @param source The source of the packet (in this case, it is SDR)
     * @param[in] packetUpperHexString QString that represents the data contained in the packet as an UPPERCASE Hex
     * QString
     */
    void dataReady(QDateTime timestamp, QString source, QString packetUpperHexString);

    /**
     * @brief Signal that is emitted when the program could not connect to a SDR, e.g. there are not any.
     */
    void cannotConnectToSDR();

    /**
     * @brief Signal that is emitted when the SDR has been disconnected during an opertaion.
     */
    void SDRHasBeenDisconnected();

    /**
     * @brief Signal that is emitted when the SDR could not be started.
     */
    void SDRWasntStarted();

public slots:
    void start(int device_index,
        long samplesPerSecond,
        int ppm,
        int gain,
        unsigned int dataRate,
        int offset,
        float df,
        unsigned int packetLength);
    void stop();
    void terminate();
    void newBaseFrequency(unsigned long frequencyHz);
};

#endif // SDRWORKER_H
