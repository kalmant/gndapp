#include "radio.h"

/**
 * @brief Radio constructor
 *
 * Initializes \p sph_prot with \p msecsBetweenCommands and sets up connections with it. Also initializes connections
 * with \p predicter.
 *
 * @param predicter Pointer to the PredicterController object
 * @param msecsBetweenCommands Milliseconds between commands for \p sph_prot
 * @param readsFromSerialPort True if the serial port is also read.
 * @param parent Parent QObject (nullptr)
 */
Radio::Radio(PredicterController *predicter, int msecsBetweenCommands, bool readsFromSerialPort, QObject *parent)
: QObject(parent) {
    Q_ASSERT(predicter != nullptr);
    sph_prot.reset(new SerialPortHandler(msecsBetweenCommands, readsFromSerialPort));

    QObject::connect(this, &Radio::newCommand, sph_prot.data(), &SerialPortHandler::newCommandSlot);
    QObject::connect(sph_prot.data(), &SerialPortHandler::sendCommandError, this, &Radio::serialPortError);
    QObject::connect(this, &Radio::changePortSettings, sph_prot.data(), &SerialPortHandler::changePortSettings);
    QObject::connect(this, &Radio::clearCommandList, sph_prot.data(), &SerialPortHandler::clearCommandList);
    QObject::connect(this, &Radio::closePortSignal, sph_prot.data(), &SerialPortHandler::closePort);
    QObject::connect(predicter, &PredicterController::trackingDataSignal, this, &Radio::trackingDataSlot);

    emit clearCommandList();
}

/**
 * @brief Starts the radio.
 *
 * Sets up the serial connection and performs additional initialization using Radio::initialization().
 * If \p turnOnOff is true then it also turns on the radio by calling Radio::turnOn()
 *
 * @param portName Name of the serial port
 * @param baudRate Baud rate as a number
 * @param offsetHz Manual offset as Hz
 * @param turnOnOff True if the radio should turn on and off during operation.
 */
void Radio::start(QString portName, int baudRate, long offsetHz, bool turnOnOff) {
    emit clearCommandList();

    portName_prot = portName;
    setPortSettingsBasedOnBaudRate(baudRate);
    previousElevation_prot = -181;
    turnOnOff_prot = turnOnOff;
    offsetHz_prot = offsetHz;

    emit changePortSettings(portName_prot, portSettings_prot);

    if (turnOnOff_prot) {
        turnOn();
    }

    initialization();

    if (turnOnOff_prot) {
        turnOff();
    }

    setIsRunning(true);
}

/**
 * @brief Turns the radio off if \p turnOnOff_prot and stops controlling it.
 */
void Radio::stop() {
    turnOff();
    QTimer::singleShot(1000, [&](){
        emit closePortSignal();
        setIsRunning(false);
    });
}

void Radio::sendCustomCommand(QByteArray command) {
    emit newCommand(command);
}

bool Radio::isRunning() const {
    return isRunning_prot;
}

void Radio::setIsRunning(bool newValue) {
    if (newValue != isRunning_prot) {
        isRunning_prot = newValue;
    }
}

/**
 * @brief Slot that handles errors regarding the usage of the serial port. Stops the radio using Radio::stop()
 * @param error The error.
 */
void Radio::serialPortError(QSerialPort::SerialPortError error) {
    qWarning() << error; // Writing the error as warning
    stop();
    emit serialPortErrorSignal();
}

/**
 * @brief Performs the necessary actions upon receiving tracking data.
 *
 * Turns the radio off using Radio::turnOff() if \p elevation goes below -5.
 * Turns the radio on using Radio::turnOff() if \p elevation rises above or reaches -5.
 * Calls Radio::setFrequency() with ( \p downlink_freq + Radio::offsetHz_prot + Radio::baseOffset_prot ) if \p elevation
 * is high enough (>= -5).
 *
 * @param azimuth UNUSED
 * @param elevation Elevation of the satellite.
 * @param downlink_freq Dowlink frequency for the satellite. Changes due to Doppler effect.
 * @param uplink_freq UNUSED
 * @param doppler100 UNUSED
 * @param nextAOSQS UNUSED
 * @param nextLOSQS UNUSED
 */
void Radio::trackingDataSlot(double azimuth,
    double elevation,
    double downlink_freq,
    double uplink_freq,
    double doppler100,
    QString nextAOSQS,
    QString nextLOSQS) {
    (void) azimuth;
    (void) uplink_freq;
    (void) doppler100;
    (void) nextAOSQS;
    (void) nextLOSQS;

    if (!isRunning()) {
        return;
    }

    int elev = static_cast<int>(elevation);
    if (elev != previousElevation_prot) {
        if (elev >= -5 && previousElevation_prot < -5) {
            turnOn();
        }
        else if (elev < -5 && previousElevation_prot >= -5) {
            turnOff();
        }
        previousElevation_prot = elev;
    }
    if (elev >= -5) {
        setFrequency(static_cast<unsigned long>(downlink_freq + offsetHz_prot + baseOffset_prot));
    }
}

/**
 * @brief Slot for new base frequency data
 * @param frequencyHz new base frequency
 */
void Radio::newBaseFrequency(unsigned long frequencyHz) {
    if (baseFrequency_prot != frequencyHz) {
        baseFrequency_prot = frequencyHz;
    }
}
