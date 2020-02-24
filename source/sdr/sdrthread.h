#ifndef SDRTHREAD_H
#define SDRTHREAD_H

#include "../../3rdparty/rtlsdr/convenience.h"
#include "../packet/packetdecoder.h"
#include "../predict/predictercontroller.h"
#include "sdrworker.h"
#include <QScopedPointer>
#include <QStringListModel>
#include <QThread>

/**
 * @brief The custom QThread subclass that hosts the SDRWorker object that demodulates signal from SDR.
 *
 * It also owns the pointers used for synchronization and communication with the SDRWorker object
 */
class SDRThread : public QThread {
    Q_OBJECT
public:
    SDRThread(PacketDecoder *pd, PredicterController *predicter);
    ~SDRThread();

    Q_PROPERTY(QStringListModel *sdrDevices READ sdrDevices NOTIFY sdrDevicesChanged)
    Q_PROPERTY(long currentFrequency READ currentFrequency NOTIFY currentFrequencyChanged)

    Q_INVOKABLE void startReading(int device_index, double ppm, int gain, bool automaticDF);
    Q_INVOKABLE void stopReading();
    Q_INVOKABLE void setOffset(long offset);
    void terminateWorker();
    Q_INVOKABLE void setDopplerFrequency(int newDF);
    Q_INVOKABLE void refreshSdrDevices();

    QStringListModel sdrDevicesModel;

    QStringListModel *sdrDevices();

    long currentFrequency();

private:
    QScopedPointer<SDRWorker> sdrWorker; //!< Pointer to the SDRWorker that demodulates the incoming signal.
    QScopedPointer<QMutex> mut_priv;     //!< Pointer to the QMmutex that is used to handle multi-thread execution.
    QScopedPointer<bool> canRun_priv;    //!< Pointer to the bool that indicates whether the device should be read.
    QScopedPointer<int>
        ds_priv; //!< Pointer to the float that lets SDRWorker know how much the dynamic shift frequency is.
    QScopedPointer<long> pl_priv;
    QScopedPointer<long> dr_priv;
    int df_priv;                             //!< Current doppler frequency
    unsigned int dataRateBPS_priv = 0;       //!< Current datarate [BPS]
    unsigned int packetLengthBytes_priv = 0; //!< Current packet length [bytes]
    bool canRun_mirror_priv;                 //!< Stores whether the SDRWorker is currently running
    bool automaticDF_priv;               //!< True if doppler frequency is automatically controlled (through tracking)
    unsigned long baseFrequency_priv;    //!< The base frequency for the communication
    long baseOffset_priv;                //!< The base offset for the satellite on top of the base frequency
    long offset_priv;                    //!< The offset that the user can set manually
    long dynamic_shift_priv;             //!< The current dynamic shift frequency
    void refreshDynamicShiftFrequency(); //!< Refreshes dynamic shift frequency in worker if necessary

public slots:
    void cannotConnectToSDRSlot();
    void SDRHasBeenDisconnectedSlot();
    void SDRWasntStartedSlot();
    void trackingDataSlot(double azimuth,
        double elevation,
        double downlink_freq,
        double uplink_freq,
        double doppler100,
        QString nextAOSQS,
        QString nextLOSQS);
    void newBaseFrequenciesSlot(unsigned long baseFrequency, long baseOffset);

signals:
    /**
     * @brief Signal that lets \p sdrWorker know that is hould start processing data.
     * @param[in] device_index Device index of the SDR.
     * @param[in] samplesPerSecond Samping rate for SDR.
     * @param[in] ppm PPM error for the SDR.
     * @param[in] gain Gain for the SDR.
     */
    void startSignal(int device_index, unsigned int samplesPerSecond, double ppm, int gain);

    /**
     * @brief The signal that lets SDRWorker know that it should stop. Possibly not useful since \p canRun_priv achieces
     * the same thing.
     */

    void stopSignal();

    /**
     * @brief Signal that is emitted when the program could not connect to a SDR.
     */
    void cannotConnectToSDRSignal();

    /**
     * @brief Signal that is emitted when the SDR has been disconnected during operation.
     */
    void sdrDisconnected();

    void sdrWasntStarted();

    void decodablePacketReceivedSignal(QDateTime timestamp, QString source, QString packetUpperHexString);

    void newBaseFrequencies(unsigned long baseFrequency, long baseOffset);

    void sdrDevicesChanged();

    void invalidSdrDeviceIndex();

    void currentFrequencyChanged();

    void complexSampleReady(std::complex<float> sample);

private slots:
    void newDataRateSlot(unsigned int newDataRateBPS);
    void newPacketLengthSlot(unsigned int newPacketLengthBytes);
    void decodablePacketReceivedSlot(QDateTime timestamp, QString source, QString packetUpperHexString);
};

#endif // SDRTHREAD_H
