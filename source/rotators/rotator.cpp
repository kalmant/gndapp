#include "rotator.h"

/**
 * @brief Rotator constructor
 *
 * Initializes \p sph_prot with \p msecsBetweenCommands and sets up connections with it. Also initializes connections
 * with \p predicter.
 *
 * @param predicter Pointer to the PredicterController object
 * @param msecsBetweenCommands Milliseconds between commands for \p sph_prot
 * @param parent Parent QObject (nullptr)
 */
Rotator::Rotator(PredicterController *predicter, int msecsBetweenCommands, QObject *parent) : QObject(parent) {
    Q_ASSERT(predicter != nullptr);
    sph_prot.reset(new SerialPortHandler(msecsBetweenCommands));

    QObject::connect(this, &Rotator::newCommand, sph_prot.data(), &SerialPortHandler::newCommandSlot);
    QObject::connect(sph_prot.data(), &SerialPortHandler::sendCommandError, this, &Rotator::serialPortError);
    QObject::connect(this, &Rotator::changePortSettings, sph_prot.data(), &SerialPortHandler::changePortSettings);
    QObject::connect(this, &Rotator::clearCommandList, sph_prot.data(), &SerialPortHandler::clearCommandList);
    QObject::connect(this, &Rotator::closePortSignal, sph_prot.data(), &SerialPortHandler::closePort);
    QObject::connect(&prePositionTimer_prot, &QTimer::timeout, [&]() { isCurrentlyFollowing_prot = true; });
    QObject::connect(this, &Rotator::nextPassDataRequestSignal, predicter, &PredicterController::requestNextPass);
    QObject::connect(predicter, &PredicterController::nextPassSignal, this, &Rotator::nextPassDataSlot);
    QObject::connect(predicter, &PredicterController::trackingDataSignal, this, &Rotator::trackingDataSlot);

    emit clearCommandList();
}

/**
 * @brief Starts controlling the rotator.
 * Initializes variables, and calls Radio::initialization() and parks the rotator if \p shouldPark is true by calling
 * Rotator::park(). Emits Rotator::nextPassDataRequestSignal() as its last instruction.
 * @param portName Name of the serial port
 * @param baudRate Baud rate as a number
 * @param parkingAz Parking azimuth [°]
 * @param parkingEl Parking elevation [°]
 * @param stoppingPosAz Azimuth point that the rotator can not cross (< 0 if there is no such point)
 * @param canElev180 True if the rotator can reach 180° elevation
 * @param shouldPark True if the rotator shold park between passes
 */
void Rotator::start(QString portName,
    int baudRate,
    unsigned int parkingAz,
    unsigned int parkingEl,
    int stoppingPosAz,
    bool canElev180,
    bool shouldPark) {
    emit clearCommandList();

    portName_prot = portName;
    setPortSettingsBasedOnBaudRate(baudRate);
    parkingAz_prot = parkingAz;
    parkingElev_prot = parkingEl;
    isElev180Capable_prot = canElev180;
    if (isElev180Capable_prot) {
        stoppingPositionAz_prot = stoppingPosAz;
    }
    else {
        stoppingPositionAz_prot = -1;
    }
    shouldPark_prot = shouldPark;
    passCrossesStopPoint_prot = false;
    isCurrentlyFollowing_prot = false;
    lastElevation_prot = -181;

    emit changePortSettings(portName_prot, portSettings_prot);

    if (shouldPark_prot) {
        park();
    }

    initialization();

    setIsRunning(true);

    emit nextPassDataRequestSignal();
}

/**
 * @brief Parks the rotator if \p shouldPark_prot is true and stop controlling it.
 */
void Rotator::stop() {
    if (shouldPark_prot) {
        park();
    }
    emit closePortSignal();
    setIsRunning(false);
    isCurrentlyFollowing_prot = false;
    prePositionTimer_prot.stop();
}

void Rotator::sendCustomCommand(QByteArray command) {
    emit newCommand(command);
}

bool Rotator::isRunning() const {
    return isRunning_prot;
}

void Rotator::setIsRunning(bool newValue) {
    if (newValue != isRunning_prot) {
        isRunning_prot = newValue;
    }
}

/**
 * @brief Moves the rotator to \p parkingAz_prot , \p parkingElev_prot
 */
void Rotator::park() {
    setAZEL(parkingAz_prot, parkingElev_prot);
}

/**
 * @brief Slot that handles errors regarding the usage of the serial port. Stops the rotator using Rotator::stop()
 * @param error The error.
 */
void Rotator::serialPortError(QSerialPort::SerialPortError error) {
    qWarning() << error; // Writing the error as warning
    stop();
    emit serialPortErrorSignal();
}

/**
 * @brief Performs the necessary actions upon receiving tracking data.
 *
 * Follows the satellite if it's above -3° and there will be an AOS.
 * Sends the rotator to parking position if \p shouldPark_prot is true.
 *
 * @param azimuth Azimuth of the satellite.
 * @param elevation Elevation of the satellite.
 * @param downlink_freq UNUSED
 * @param uplink_freq UNUSED
 * @param doppler100 UNUSED
 * @param nextAOSQS UNUSED
 * @param nextLOSQS UNUSED
 */
void Rotator::trackingDataSlot(double azimuth,
    double elevation,
    double downlink_freq,
    double uplink_freq,
    double doppler100,
    QString nextAOSQS,
    QString nextLOSQS) {
    (void) downlink_freq;
    (void) uplink_freq;
    (void) doppler100;
    (void) nextAOSQS;
    (void) nextLOSQS;

    if (!isRunning()) {
        return;
    }

    int elev = static_cast<int>(elevation);
    if (!isCurrentlyFollowing_prot && elev >= 0) {
        isCurrentlyFollowing_prot = true;
    }

    if (isCurrentlyFollowing_prot) {
        if (lastElevation_prot >= -3 && elev <= -3) {
            isCurrentlyFollowing_prot = false;
            if (shouldPark_prot) {
                park();
            }
            prePositionTimer_prot.stop();
            emit nextPassDataRequestSignal();
        }
        else {
            unsigned int el = elev < 0 ? 0 : static_cast<unsigned int>(elev);
            unsigned int az = static_cast<unsigned int>(azimuth);
            if (passCrossesStopPoint_prot) {
                setAZEL((az + 180) % 360, 180 - el);
            }
            else {
                setAZEL(az, el);
            }
        }
    }

    if (lastElevation_prot != elev) {
        lastElevation_prot = elev;
    };
}

/**
 * @brief Makes sure that if the next pass's azimuth crosses \p stoppingPositionAz_prot then the \p
 * passCrossesStopPoint_prot is set to true so that the rotator can
 * @param nextPassAzimuths
 * @param nextPassTimestamp
 */
void Rotator::nextPassDataSlot(QList<unsigned int> nextPassAzimuths, long nextPassTimestamp) {
    passCrossesStopPoint_prot = false;
    if (stoppingPositionAz_prot >= 0) {
        unsigned int azStop = static_cast<unsigned int>(stoppingPositionAz_prot);
        unsigned int prevAzimuth = nextPassAzimuths.first();
        for (unsigned int azimuth : nextPassAzimuths) {
            if ((prevAzimuth <= azStop && azStop <= azimuth) || (prevAzimuth >= azStop && azStop >= azimuth)) {
                passCrossesStopPoint_prot = true;
                break;
            }
            prevAzimuth = azimuth;
        }
    }

    long secondsUntilNextPass = nextPassTimestamp - static_cast<long>((QDateTime::currentMSecsSinceEpoch() / 1000));
    if (secondsUntilNextPass < 180) {
        isCurrentlyFollowing_prot = true;
    }
    else {
        prePositionTimer_prot.start(secondsUntilNextPass * 1000);
    }
}
