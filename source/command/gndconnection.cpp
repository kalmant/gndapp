#include "gndconnection.h"

#ifdef UPLINK_ENABLED

// Not included in this version

#endif // UPLINK_ENABLED

/**
 * @brief Constructor for the class that initializes \p tcpSocket and connects
 * signals/slots.
 * @param parent
 */
GNDConnection::GNDConnection(QObject *parent) : QObject(parent) {
    tcpSocket.reset(new QTcpSocket(this));
    commands_priv.setCommandId(0);
    this->currentSatellite = SatelliteChanger::Satellites::UNSET;
    connect(&commandQueueTimer_priv, &QTimer::timeout, [this]() {
        if (commands_priv.isQueueEmpty()) {
            this->stopTimers();
            return;
        }
        if (tcpSocket->state() == QTcpSocket::ConnectedState) {
            auto nextCommand = commands_priv.takeCommand(QDateTime::currentDateTimeUtc());
            if (std::get<1>(nextCommand).isEmpty()) {
                // The command list is not empty, but neither one of the queued commands can be sent out yet
                return;
            }
            sendCommand(std::get<0>(nextCommand), std::get<1>(nextCommand), std::get<2>(nextCommand));
            emit commandListChanged();
        }
        else {
            this->stopTimers();
            qWarning() << "Could not send command because the socket is not connected";
        }
    });

    connect(&commandTimeoutTimer_priv, &QTimer::timeout, [this]() { this->stopTimers(); });

    connect(tcpSocket.data(), &QTcpSocket::connected, this, &GNDConnection::connectedToServer);
    connect(tcpSocket.data(), &QTcpSocket::disconnected, this, &GNDConnection::disconnectedFromServer);
    connect(tcpSocket.data(), &QTcpSocket::bytesWritten, this, &GNDConnection::bytesWritten);
    connect(tcpSocket.data(), &QTcpSocket::readyRead, this, &GNDConnection::readyRead);
    connect(tcpSocket.data(),
        static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
        this,
        &GNDConnection::error);
    connect(tcpSocket.data(), &QTcpSocket::stateChanged, this, &GNDConnection::isConnectedChanged);
}

/**
 * @brief Disconnects \p tcpSocket from the host and calls deleteLater() on it.
 */
GNDConnection::~GNDConnection() {
    if (tcpSocket != nullptr) {
        tcpSocket->disconnectFromHost();
    }
}

/**
 * @brief Returns whether a connection has been established.
 * @return Returns true if \p tcpSocket is in QAbstractSocket::ConnectedState,
 * returns false otherwise.
 */
bool GNDConnection::isConnected() const {
    return (tcpSocket->state() == QAbstractSocket::ConnectedState);
}

/**
 * @brief Connects to the IP \p ipAddress and through the port \p port.
 * @param ipAddress The port to connect to.
 * @param port The port to use when connecting.
 */
void GNDConnection::connectToServer(QString ipAddress, quint16 port) {
    qInfo() << "Connecting to the server";

    tcpSocket->connectToHost(ipAddress, port);
}

/**
 * @brief Disconnets from the host.
 */
void GNDConnection::disconnectFromServer() {
    tcpSocket->disconnectFromHost();
}

/**
 * @brief Sends out \p command through GNDConnection::tcpSocket.
 * @param commandId The id of the command
 * @param command The command itself
 * @param commandString A string representing the command
 */
void GNDConnection::sendCommand(uint16_t commandId, QByteArray command, QString commandString) {
#ifdef UPLINK_ENABLED
    // Not included in this version
    Q_UNUSED(commandId)
    Q_UNUSED(command)
    Q_UNUSED(commandString)
#else
    Q_UNUSED(commandId)
    Q_UNUSED(command)
    Q_UNUSED(commandString)
    qWarning() << "You can not send commands without having UPLINK_ENABLED";
#endif // UPLINK_ENABLED
}

/**
 * @brief Queues \p command \p times with \p commandString
 * @param times The command will be placed this many times into \p commands_priv
 * @param command The command to queue
 * @param commandString String describing the command
 * @param timestamp The QDateTime timestamp, that the command should not be sent out before
 */
void GNDConnection::queueCommand(unsigned int times, QByteArray command, QString commandString, QDateTime timestamp) {
#ifdef UPLINK_ENABLED
    commands_priv.addToQueue(times, command, commandString, timestamp.toUTC());
    emit commandListChanged();
#else
    Q_UNUSED(times)
    Q_UNUSED(command)
    Q_UNUSED(commandString)
    Q_UNUSED(timestamp)
    qWarning() << "You can not send commands without having UPLINK_ENABLED";
#endif // UPLINK_ENABLED
}

/**
 * @brief Stops \p commandQueueTimer_priv and \p commandTimeoutTimer_priv
 */
void GNDConnection::stopTimers() {
    commandQueueTimer_priv.stop();
    commandTimeoutTimer_priv.stop();
}
/**
 * @brief Enqueues an uplink command to be sent \p repeat times.
 * @param repeat The command will be eventually sent this many times.
 * @param sendTimestamp When the command will be sent out
 * @param command The command to send, must contain a string or an uplink packet object.
 */
void GNDConnection::queueUplink(quint32 repeat, quint64 sendTimestamp64, QVariant command) {
    if (command.isNull() || !command.isValid()) {
        qWarning() << "invalid command";
        return;
    }

    QDateTime sendTimestamp = QDateTime::fromSecsSinceEpoch(sendTimestamp64);
    QString commandString;
    QByteArray commandBytes;

    if (command.type() == QVariant::String) {
        commandString = QStringLiteral("Custom");
        commandBytes = command.toString().toLatin1();

        if (commandBytes.length() < OUTPUTLENGTH) {
            commandBytes.append((OUTPUTLENGTH - commandBytes.length()), 0);
        }
    }
    else if (command.canConvert<s1obc::UplinkPingPacket>()) {
        auto packet = command.value<s1obc::UplinkPingPacket>();
        commandString = QStringLiteral("Ping");
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkTogglePacket>()) {
        auto packet = command.value<s1obc::UplinkTogglePacket>();
        QString unit;
        switch (packet.toggleType()) {
        case s1obc::UplinkToggleType_None:
            unit = QString("nothing");
            break;
        case s1obc::UplinkToggleType_COM:
            unit = QString("COM");
            break;
        case s1obc::UplinkToggleType_OBC:
            unit = QString("OBC");
            break;
        case s1obc::UplinkToggleType_SDC:
            unit = QString("SDC");
            break;
        case s1obc::UplinkToggleType_Battery:
            unit = QString("Battery");
            break;
        }
        commandString = QStringLiteral("Toggle ") + unit;
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkResetPacket>()) {
        auto packet = command.value<s1obc::UplinkResetPacket>();
        QString unit;
        switch (packet.resetType()) {
        case s1obc::UplinkResetType_None:
            unit = QString("nothing");
            break;
        case s1obc::UplinkResetType_COM:
            unit = QString("COM");
            break;
        case s1obc::UplinkResetType_OBC:
            unit = QString("OBC");
            break;
        case s1obc::UplinkResetType_PCU:
            unit = QString("PCU");
            break;
        case s1obc::UplinkResetType_TID:
            unit = QString("TID");
            break;
        case s1obc::UplinkResetType_OBC_UserData:
            unit = QString("OBC user data");
            break;
        case s1obc::UplinkResetType_ExitSilentMode:
            unit = QString("Exit silent mode");
            break;
        }
        commandString = QStringLiteral("Reset ") + unit;
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkSetComParametersPacket>()) {
        auto packet = command.value<s1obc::UplinkSetComParametersPacket>();
        QString txpl;
        switch (packet.txPowerLevel()) {
        case s1obc::ComTxPowerLevel_10mW:
            txpl = QString("10 mW (0x00)");
            break;
        case s1obc::ComTxPowerLevel_1:
            txpl = QString("?? mW (0x01)");
            break;
        case s1obc::ComTxPowerLevel_2:
            txpl = QString("?? mW (0x02)");
            break;
        case s1obc::ComTxPowerLevel_3:
            txpl = QString("?? mW (0x03)");
            break;
        case s1obc::ComTxPowerLevel_4:
            txpl = QString("?? mW (0x04)");
            break;
        case s1obc::ComTxPowerLevel_5:
            txpl = QString("?? mW (0x05)");
            break;
        case s1obc::ComTxPowerLevel_6:
            txpl = QString("?? mW (0x06)");
            break;
        case s1obc::ComTxPowerLevel_25mW:
            txpl = QString("25 mW (0x07)");
            break;
        case s1obc::ComTxPowerLevel_8:
            txpl = QString("?? mW (0x08)");
            break;
        case s1obc::ComTxPowerLevel_9:
            txpl = QString("?? mW (0x09)");
            break;
        case s1obc::ComTxPowerLevel_A:
            txpl = QString("?? mW (0x0A)");
            break;
        case s1obc::ComTxPowerLevel_B:
            txpl = QString("?? mW (0x0B)");
            break;
        case s1obc::ComTxPowerLevel_C:
            txpl = QString("?? mW (0x0C)");
            break;
        case s1obc::ComTxPowerLevel_50mW:
            txpl = QString("50 mW (0x0D)");
            break;
        case s1obc::ComTxPowerLevel_E:
            txpl = QString("?? mW (0x0E)");
            break;
        case s1obc::ComTxPowerLevel_100mW:
            txpl = QString("100 mW (0x0F)");
            break;
        }
        QString txdr;
        switch (packet.txDataRate()) {
        case s1obc::ComTxDatarate_500bps:
            txdr = QString("500 bps");
            break;
        case s1obc::ComTxDatarate_1250bps:
            txdr = QString("1250 bps");
            break;
        case s1obc::ComTxDatarate_2500bps:
            txdr = QString("2500 bps");
            break;
        case s1obc::ComTxDatarate_5000bps:
            txdr = QString("5000 bps");
            break;
        case s1obc::ComTxDatarate_12500bps:
            txdr = QString("12500 bps");
            break;
        case s1obc::ComTxDatarate_25000bps:
            txdr = QString("25000 bps");
            break;
        case s1obc::ComTxDatarate_50000bps:
            txdr = QString("50000 bps");
            break;
        case s1obc::ComTxDatarate_125000bps:
            txdr = QString("125000 bps");
            break;
        }
        commandString = QStringLiteral("COM set params - TXDR: ") + txdr + " TXPL:" + txpl;
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkStartSpectrumAnalyzerPacket>()) {
        auto packet = command.value<s1obc::UplinkStartSpectrumAnalyzerPacket>();
        QString frequencyRangeRBW;
        auto startFreq = packet.startFrequency();
        auto stopFreq = packet.stopFrequency();
        auto rbw = packet.rbw();
        if (startFreq == 460000000 && stopFreq == 860000000 && rbw == 8) {
            frequencyRangeRBW = QString("DVB-T");
        }
        else if (startFreq == 824000000 && stopFreq == 960000000 && rbw == 6) {
            frequencyRangeRBW = QString("GSM-900");
        }
        else if (startFreq == 144000000 && stopFreq == 146000000 && rbw == 2) {
            frequencyRangeRBW = QString("ISM-100");
        }
        else if (startFreq == 432000000 && stopFreq == 438000000 && rbw == 2) {
            frequencyRangeRBW = QString("ISM-400");
        }
        else if (startFreq == 119000000 && stopFreq == 137000000 && rbw == 5) {
            frequencyRangeRBW = QString("Air");
        }
        else {
            frequencyRangeRBW = QString("%1 - %2, RBW: %3").arg(startFreq).arg(stopFreq).arg(rbw);
        }
        QString parameters = frequencyRangeRBW + QString(", step: %1, TS: %2, ID: %3")
                                                     .arg(packet.stepSize())
                                                     .arg(packet.startTimestamp())
                                                     .arg(packet.measurementIdentifier());
        commandString = QStringLiteral("SPA: ") + parameters;
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkOpenAntennaPacket>()) {
        auto packet = command.value<s1obc::UplinkOpenAntennaPacket>();
        QString kind;
        auto pwm = packet.batteryPwmTime();
        auto charge = packet.capacitorPwmChargeTime();
        auto discharge = packet.capacitorPwmDischargeTime();
        if (pwm != 0) {
            kind = QString("- PWM duration: %1 ms").arg(pwm);
        }
        else {
            kind = QString("and capacitor - Charge: %1 ms, discharge: %2 ms").arg(charge).arg(discharge);
        }
        commandString = QStringLiteral("Open antenna from battery ") + kind;
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkSetBeaconPacket>()) {
        auto packet = command.value<s1obc::UplinkSetBeaconPacket>();
        QString description;
        auto starts = packet.startIndex();
        if (starts == 80) {
            description = QString("Clear");
        }
        else {
            description =
                QString("Starting at: %1 , message: \"").arg(starts).append(packet.beaconFragment_qt()).append("\"");
        }
        commandString = QStringLiteral("Set beacon - ") + description;
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkFileDownloadRequestPacket>()) {
        auto packet = command.value<s1obc::UplinkFileDownloadRequestPacket>();
        QString parameters;
        QString txdr;
        switch (packet.txDataRate()) {
        case s1obc::ComTxDatarate_500bps:
            txdr = QString("500 bps");
            break;
        case s1obc::ComTxDatarate_1250bps:
            txdr = QString("1250 bps");
            break;
        case s1obc::ComTxDatarate_2500bps:
            txdr = QString("2500 bps");
            break;
        case s1obc::ComTxDatarate_5000bps:
            txdr = QString("5000 bps");
            break;
        case s1obc::ComTxDatarate_12500bps:
            txdr = QString("12500 bps");
            break;
        case s1obc::ComTxDatarate_25000bps:
            txdr = QString("25000 bps");
            break;
        case s1obc::ComTxDatarate_50000bps:
            txdr = QString("50000 bps");
            break;
        case s1obc::ComTxDatarate_125000bps:
            txdr = QString("125000 bps");
            break;
        }
        parameters.append("TXDR: ").append(txdr);
        auto delay = packet.relativeTime();
        auto fileId = packet.fileId();
        auto fileName = packet.fileName_qt();
        auto shouldDelete = packet.shouldDelete();
        auto length = packet.length();
        auto offset = packet.offset();
        if (fileId != 0) {
            parameters.append(QString(", ID: %1").arg(fileId));
        }
        else {
            parameters.append(", name: \"").append(fileName).append("\"");
        }
        if (offset != 0) {
            parameters.append(QString(", offset: %1").arg(offset));
        }
        if (length != 0) {
            parameters.append(QString(", length: %1").arg(length));
        }
        if (delay != 0) {
            parameters.append(QString(", delay: %1").arg(delay));
        }
        if (shouldDelete == 1) {
            parameters.append(", delete!");
        }

        commandString = QStringLiteral("File download: ") + parameters;
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkAntennaOpenSuccessPacket>()) {
        auto packet = command.value<s1obc::UplinkAntennaOpenSuccessPacket>();
        commandString = QStringLiteral("Antenna success");
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkMeasurementRequestPacket>()) {
        auto packet = command.value<s1obc::UplinkMeasurementRequestPacket>();
        commandString = QStringLiteral("Measurement request");
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkFileDeletePacket>()) {
        auto packet = command.value<s1obc::UplinkFileDeletePacket>();
        commandString = QStringLiteral("File delete: ") + packet.fileName_qt();
        if (packet.fileNameIsFilter()) {
            commandString += QStringLiteral(" (filter)");
        }
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkMorseRequestPacket>()) {
        auto packet = command.value<s1obc::UplinkMorseRequestPacket>();
        commandString = QStringLiteral("Morse request: ") + packet.morseMessage_qt();
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }
    else if (command.canConvert<s1obc::UplinkSilentModePacket>()) {
        auto packet = command.value<s1obc::UplinkSilentModePacket>();
        commandString = QStringLiteral("Silent mode: ") + packet.silentDuration();
        commandBytes = QByteArray(reinterpret_cast<const char *>(packet.binary()), s1obc::MaxUplinkPayloadSize);
    }

    if (commandBytes.length() == 0) {
        qWarning() << "unknown command";
        return;
    }
    else if (commandBytes.length() != s1obc::MaxUplinkPayloadSize && commandBytes.length() != OUTPUTLENGTH) {
        qWarning() << "invalid command length";
        return;
    }

    qWarning() << QString::fromLatin1(commandBytes.toHex(' '));
    queueCommand(repeat, commandBytes, commandString, sendTimestamp);
}

/**
 * @brief Clears \p commands_priv
 */
void GNDConnection::clearCommandQueue() {
    commands_priv.clearQueue();
    emit commandListChanged();
}

/**
 * @brief Returns the command list from \p commands_priv
 * @return
 */
QStringList GNDConnection::commandList() const {
    return commands_priv.getCommandList();
}

/**
 * @brief Slot that receives a signal when a connection has been established by
 * \p tcpSocket.
 */
void GNDConnection::connectedToServer() {
    qInfo() << "Successfully connected to the server";
    setDatarate(this->dataRate_priv);
    setPacketLength(this->packetLength_priv);
}

/**
 * @brief Slot that receives a signal when a connection has been terminated by
 * \p tcpSocket.
 */
void GNDConnection::disconnectedFromServer() {
    qInfo() << "Disconnected from the server";
}

/**
 * @brief Slot that receives a signal when a message has been sent through \p
 * tcpSocket.
 * @param bytes The bytes written to \p tcpSocket.
 */
void GNDConnection::bytesWritten(qint64 bytes) {
    qInfo() << bytes << "bytes written";
}

/**
 * @brief Slot that receives a signal when a message has arrived to \p
 * tcpSocket.
 *
 * Splits the available data to separate messages and emits
 * GNDConnection::packetReceived() , GNDConnection::decodablePacket() .
 *
 */
void GNDConnection::readyRead() {
    QString contents = QString(tcpSocket->readAll());
    receivedMessage_priv += contents;

    int nl;

    while ((nl = receivedMessage_priv.indexOf(QChar('\n'))) >= 0) {
        // We don't need the \n at the end
        QString msg = receivedMessage_priv.mid(0, nl);
        msg = msg.trimmed();
        receivedMessage_priv = receivedMessage_priv.mid(nl + 1);

        if (msg.length() > 0) {
            if (msg.startsWith(QStringLiteral("log file open"))) {
                continue;
            }
            if (msg.startsWith(QStringLiteral("CTS error"))) {
                continue;
            }

            processMessage(msg);
        }
    }

    if (receivedMessage_priv.length()) {
        qInfo() << "could not yet process" << receivedMessage_priv;
    }
}

void GNDConnection::processMessage(const QString &msg) {
    QDateTime timestamp = QDateTime::currentDateTimeUtc();
    QStringList msgParts = msg.split(" ");
    QString message = msgParts.at(0);
    int64_t expectedMessageLength = this->packetLength_priv * 2;
    int rssi = 0;

    if (msgParts.length() >= 9 && msgParts.at(1).compare("RSSI") == 0) {
        // additional information from GND
        rssi = msgParts.at(2).toInt();
    }

    if (message.length() == 0) {
        return;
    }
    else if ((message.length() % 2) != 0) {
        qWarning() << "Invalid message length, should be an even number, but is:" << message.length();
        setDatarate(1250);
        setPacketLength(70);
    }
    else if (static_cast<unsigned int>(message.length()) < expectedMessageLength) {
        if (message.length() > 40) {
            // command echo
        }
        else {
            // message from GND
        }
    }
    else if (static_cast<unsigned int>(message.length()) == expectedMessageLength) {
        //(hopefully) packet from the satellite, as a hexadecimal string
        emit decodablePacket(timestamp, "GNDConnection", message, rssi);
    }
    else {
        // GND sent a message that is too long
        qWarning() << "Wrong message length (" << message.length() << ") for packet: " << message
                   << "expected:" << expectedMessageLength;
        setDatarate(1250);
        setPacketLength(70);
    }
}

/**
 * @brief Starts sending commands in \p commands_priv.
 * @param startDelayMs Initial delay before the commands are sent.
 * @param timeBetweenMs Time between commands.
 * @param stopAfterMilliseconds Commands are no longer sent after this many
 * milliseconds.
 */
void GNDConnection::startSendingCommands(
    unsigned long startDelayMs, unsigned int timeBetweenMs, unsigned long stopAfterMilliseconds) {
#ifdef UPLINK_ENABLED
    stopTimers();
    QTimer::singleShot(startDelayMs, [timeBetweenMs, stopAfterMilliseconds, this]() {
        commandQueueTimer_priv.start(static_cast<int>(timeBetweenMs));
        commandTimeoutTimer_priv.start(static_cast<int>(stopAfterMilliseconds));
    });
#else
    Q_UNUSED(startDelayMs)
    Q_UNUSED(timeBetweenMs)
    Q_UNUSED(stopAfterMilliseconds)
    return; // commands are not sent if UPLINK_ENABLED is not set
#endif // UPLINK_ENABLED
}

/**
 * @brief Calls CommandQueue::setCommandId() on \p commands_priv with \p newId as parameter
 * @param newId New command id
 */
void GNDConnection::newCommandIdSlot(uint16_t newId) {
    commands_priv.setCommandId(newId);
}

/**
 * @brief Slot that receives a signal when there was an error with \p tcpSocket.
 * @param socketError The QAbstractSocket::SocketError error that has occured.
 */
void GNDConnection::error(QAbstractSocket::SocketError socketError) {
    qWarning() << "Connection error:" << socketError;
}

/**
 * @brief Sends the appropriate command to \p tcpSocket based on \p datarateBPS
 * @param datarateBPS The new datarate
 */
void GNDConnection::setDatarate(unsigned int datarateBPS) {
    this->dataRate_priv = datarateBPS;
    emit newDatarate(dataRate_priv);
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        switch (this->dataRate_priv) {
        case 500:
            tcpSocket->write(QByteArray("a\n"));
            break;
        case 1250:
            tcpSocket->write(QByteArray("b\n"));
            break;
        case 2500:
            tcpSocket->write(QByteArray("c\n"));
            break;
        case 5000:
            tcpSocket->write(QByteArray("d\n"));
            break;
        case 12500:
            tcpSocket->write(QByteArray("e\n"));
            break;
        case 25000:
            tcpSocket->write(QByteArray("f\n"));
            break;
        case 50000:
            tcpSocket->write(QByteArray("g\n"));
            break;
        case 125000:
            tcpSocket->write(QByteArray("h\n"));
            break;
        default:
            qWarning() << "Invalid TXDR in SETCOM/FileDownloadRequest - did not send new TXDR to ground station!";
            return;
        }
        qInfo() << "Sent new datarate: " + QString::number(datarateBPS);
    }
}

/**
 * @brief Sends the appropriate command to \p tcpSocket based on \p packetLength
 * @param packetLength The new packet length
 */
void GNDConnection::setPacketLength(unsigned int packetLength) {
    this->packetLength_priv = packetLength;
    emit newPacketLength(packetLength_priv);
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        tcpSocket->write(QByteArray(QString("i%1\n").arg(packetLength, 4, 10, QChar('0')).toLocal8Bit()));
        qInfo() << "Sent new packet length to GND:" + QString::number(packetLength);
    }
}

void GNDConnection::changeSatellite(SatelliteChanger::Satellites satellite) {
    this->currentSatellite = satellite;
    // Command id is reset upon satellite change: different satellites may have different ids
    commands_priv.setCommandId(0);
}
