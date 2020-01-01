#ifndef RADIO_H
#define RADIO_H

#include "../predict/predictercontroller.h"
#include "../utilities/serialporthandler.h"
#include <QObject>
#include <QScopedPointer>

class Radio : public QObject {
    Q_OBJECT
public:
    explicit Radio(PredicterController *predicter,
        int msecsBetweenCommands,
        bool readsFromSerialPort = false,
        QObject *parent = nullptr);

    Q_INVOKABLE void start(QString portName, int baudRate, long offsetHz, bool turnOnOff = true);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void sendCustomCommand(QByteArray command);

protected:
    QScopedPointer<SerialPortHandler> sph_prot; //!< QSPointer to the SerialPortHandler object
    SerialPortSettings portSettings_prot;       //!< The current serial port settings as a SerialPortSettings object
    QString portName_prot;                      //!< Name of the currently used serial port
    bool isRunning_prot = false;                //!< Variable that holds whether the radio is running
    long offsetHz_prot = 0;                     //!< Manual offset for the radio
    long baseOffset_prot = 0;                   //!< Additional offset (e.g. 1500 Hz for FT817 & TS2000)
    bool turnOnOff_prot;                        //!< True if the radio should be turned on and off between passes.
    int previousElevation_prot;                 //!< Previous elevation for the satellite
    unsigned long previousFrequency_prot = 0;   //!< Previous frequency for the satellite
    unsigned long baseFrequency_prot = INITIALBASEFREQUENCY; //!< The base frequency for the satellite

    bool isRunning() const;
    void setIsRunning(bool newValue);

    /**
     * @brief Sets the radio to \p frequencyHz
     * @param frequencyHz The frequency that the radio will be set to.
     */
    virtual void setFrequency(unsigned long frequencyHz) = 0;

    /**
     * @brief Sets \p portSettings_prot based on \p baudRate and the model's specification.
     * @param baudRate The Baud rate (as a number)
     */
    virtual void setPortSettingsBasedOnBaudRate(int baudRate) = 0;

    /**
     * @brief Performs radio specific initialization operations.
     */
    virtual void initialization() = 0;

    /**
     * @brief Turns the radio off.
     */
    virtual void turnOff() = 0;

    /**
     * @brief Turns the radio on.
     */
    virtual void turnOn() = 0;

signals:
    /**
     * @brief The signal that is used to send commands to the radio. It is caught by \p sph_prot.
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

public slots:
    void serialPortError(QSerialPort::SerialPortError error);
    void trackingDataSlot(double azimuth,
        double elevation,
        double downlink_freq,
        double uplink_freq,
        double doppler100,
        QString nextAOSQS,
        QString nextLOSQS);
    void newBaseFrequency(unsigned long frequencyHz);
};

#endif // RADIO_H
