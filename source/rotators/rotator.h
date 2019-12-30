#ifndef ROTATOR_H
#define ROTATOR_H

#include "../predict/predictercontroller.h"
#include "../utilities/serialporthandler.h"
#include <QDateTime>
#include <QObject>

class Rotator : public QObject {
    Q_OBJECT
public:
    explicit Rotator(PredicterController *predicter, int msecsBetweenCommands, QObject *parent = nullptr);

    Q_INVOKABLE void start(QString portName,
        int baudRate,
        unsigned int parkingAz,
        unsigned int parkingEl,
        int stoppingPosAz,
        bool canElev180,
        bool shouldPark);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void sendCustomCommand(QByteArray command);

protected:
    QScopedPointer<SerialPortHandler> sph_prot; //!< SerialPortHandler object that handles the serial port
    SerialPortSettings portSettings_prot;       //!< The current serial port settings as a SerialPortSettings object
    QString portName_prot;                      //!< Name of the currently used serial port
    QTimer prePositionTimer_prot;  //!< Timer that starts makes sure that control is established 3 mins. before AOS
    bool isRunning_prot = false;   //!< Variable that holds whether the rotator is running
    unsigned int parkingAz_prot;   //!< Parking azimuth
    unsigned int parkingElev_prot; //!< Parking elevation
    int stoppingPositionAz_prot;   //!< Azimuth point that the rotator can not cross
    bool shouldPark_prot;          //!< True if the rotator should park between passes
    bool isElev180Capable_prot;    //!< True is the rotator is capable of 180° elevation
    bool passCrossesStopPoint_prot = false; //!< True if the next pass crosses \p stoppingPositionAz_prot
    bool isCurrentlyFollowing_prot = false; //!< True if the rotator is following the satellite ATM
    int lastElevation_prot = -181;          //!< Previous elevation received from predicter
    int lastAzimuth_prot = -361;            //!< Previous azimuth received from predicter

    bool isRunning() const;
    void setIsRunning(bool newValue);

    /**
     * @brief Sets the rotator to \p azimuth and \p elevation
     * @param azimuth Azimuth [°]
     * @param elevation Elevation [°]
     */
    virtual void setAZEL(unsigned int azimuth, unsigned int elevation) = 0;

    /**
     * @brief Sets \p portSettings_prot based on \p baudRate and the model's specification.
     * @param baudRate The Baud rate (as a number)
     */
    virtual void setPortSettingsBasedOnBaudRate(int baudRate) = 0;

    /**
     * @brief Performs rotator specific initialization operations.
     */
    virtual void initialization() = 0;

    /**
     * @brief Parks the rotator.
     */
    virtual void park();

signals:
    /**
     * @brief The signal that is used to send commands to the rotator. It is caught by \p sph_prot.
     * @param[in] command The command sent to the radio.
     */
    void newCommand(const QByteArray &command);

    /**
     * @brief The signal that is used to change the serial port settings. It is caught by \p sph_prot.
     * @param[in] portName Name of the new port that will be used.
     * @param[in] settings Settings that the port will be initialized with.
     */
    void changePortSettings(QString portName_prv, SerialPortSettings settings);

    /**
     * @brief Signal that is used to clear the command queue in \p sph_prot.
     */
    void clearCommandList();

    /**
     * @brief Signal that is emitted when the port should be closed. Caught by \p sph_prot.
     */
    void closePortSignal();

    /**
     * @brief Signal that is emitted when there has been a problem with the commucation. Currently (20180108) unused!
     */
    void serialPortErrorSignal();

    /**
     * @brief Signal that requests the next pass data
     */
    void nextPassDataRequestSignal();
public slots:
    void serialPortError(QSerialPort::SerialPortError error);
    void trackingDataSlot(double azimuth,
        double elevation,
        double downlink_freq,
        double uplink_freq,
        double doppler100,
        QString nextAOSQS,
        QString nextLOSQS);
    void nextPassDataSlot(QList<unsigned int> nextPassAzimuths, long nextPassTimestamp);
};

#endif // ROTATOR_H
