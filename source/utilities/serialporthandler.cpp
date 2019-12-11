#include "serialporthandler.h"
#include <QDateTime>

/**
 * @brief Constructor for the class
 *
 * Sets the timer member variable for the timer based on \p msecsBetweenCommands.
 * Starts a timer for SerialPortHandler::sendNextCommand() calls. It also emits SerialPortHandler::sendCommandError() if
 * there was an error.
 *
 * @param msecsBetweenCommands Milliseconds between SerialPortHandler::sendNextCommand() calls. Uses a Qt::CoarseTimer.
 * @param[in] readFromDevice True if the device should be opened for reading and writing and data read is emitted.
 * @param[in] parent Parent QObject, should be left empty
 */
SerialPortHandler::SerialPortHandler(int msecsBetweenCommands, bool readFromDevice, QObject *parent) : QObject(parent) {
    this->msecsBetweenCommands_priv = msecsBetweenCommands;
    timer_priv.start(this->msecsBetweenCommands_priv);
    QObject::connect(&timer_priv, &QTimer::timeout, [&]() {
        QSerialPort::SerialPortError err = sendNextCommand();
        if (err != QSerialPort::NoError)
            emit sendCommandError(err);
    });
    readFromDevice_priv = readFromDevice;
    if (readFromDevice_priv) {
        QObject::connect(&port_priv, &QSerialPort::readyRead, this, &SerialPortHandler::handleReadyRead);
    }
}

/**
 * @brief Initializes the port specified by /p portName with /p settings.
 *
 * Checks whether the specified port is available, initializes and opens it.
 *
 * @param[in] portName Name of the port that will be opened.
 * @param[in] settings Contains the settings that the port will be set up with.
 * @return Returns any error that it encounters during the process.
 */
QSerialPort::SerialPortError SerialPortHandler::initPort(QString portName, const SerialPortSettings settings) {
    bool portFound = false;
    QList<QSerialPortInfo> list = getPorts();
    int portIndexInList;
    buffer_priv.clear();

    if (port_priv.isOpen())
        port_priv.close();
    for (int i = 0; i < list.length(); i++) {
        if (portName.compare(list.at(i).portName(), Qt::CaseSensitive) == 0) {
            portFound = true;
            portIndexInList = i;
            break;
        }
    }
    if (portFound) {
        port_priv.setBaudRate(settings.BaudRate);
        port_priv.setParity(settings.Parity);
        port_priv.setStopBits(settings.StopBits);
        port_priv.setFlowControl(settings.FlowControl);
        port_priv.setPort(list.at(portIndexInList));
        if (readFromDevice_priv) {
            port_priv.open(QSerialPort::ReadWrite);
        }
        else {
            port_priv.open(QSerialPort::WriteOnly);
        }
    }
    else {
        return QSerialPort::DeviceNotFoundError;
    }
    return port_priv.error();
}

/**
 * @brief Reads the data from the device whenever it has emitted QSerialPort::readyRead
 */
void SerialPortHandler::handleReadyRead() {
    auto bytes = port_priv.readAll();
    buffer_priv.append(bytes);

    for (auto pos = buffer_priv.indexOf('\n'); pos > -1; pos = buffer_priv.indexOf('\n')) {
        emit dataRead(QString::fromLatin1(buffer_priv.left(pos + 1)));
        buffer_priv = buffer_priv.right(buffer_priv.length() - (pos + 1));
    }
}

/**
 * @brief Returns a list of available QSerialPortInfo's.
 * @return QList of QSerialPortInfo's.
 */
const QList<QSerialPortInfo> SerialPortHandler::getPorts() {
    return QSerialPortInfo::availablePorts();
}

/**
 * @brief Clears the list of commands to send.
 */
void SerialPortHandler::clearCommandList() {
    commandsToSend_priv.clear();
}

/**
 * @brief Clears the command list and closes the port if it's open.
 */
void SerialPortHandler::closePort() {
    clearCommandList();
    if (port_priv.isOpen()) {
        port_priv.clear();
        port_priv.close();
    }
    if (port_priv.error() != QSerialPort::NoError) {
        qWarning() << "Error occurred during the closing of the serial port";
    }
}

/**
 * @brief Send the next command from the list of commands.
 *
 * Sends the first command in the command queue. If there aren't any left or \p port_priv is not open, the timer is
 * stopped.
 *
 * @return The QSerialPort::SerialportError encountered.
 */
QSerialPort::SerialPortError SerialPortHandler::sendNextCommand() {
    if (port_priv.isOpen() && commandsToSend_priv.length() > 0) {
        auto command = commandsToSend_priv.first();
        port_priv.write(command);

        if (port_priv.error() == QSerialPort::NoError) {
            qInfo() << "command sent without error:" << command;
            commandsToSend_priv.removeFirst();
        }
        else {
            qInfo() << "error:" << port_priv.error() << "when sending:" << command;
        }

        if (commandsToSend_priv.length() == 0) {
            timer_priv.stop();
        }
        return port_priv.error();
    }
    timer_priv.stop();
    return QSerialPort::NoError;
}

/**
 * @brief A slot for adding new commands to the command queue.
 *
 * Appends the QByteArray to the end of the command queue. If the timer is not active, the timer is started.
 *
 * @param[in] command The QByteArray to send on the port.
 */
void SerialPortHandler::newCommandSlot(const QByteArray &command) {
    commandsToSend_priv.append(command);
    if (!timer_priv.isActive())
        timer_priv.start(this->msecsBetweenCommands_priv);
}

/**
 * @brief Changes parameters of the port being used and opens the connection.
 *
 * Calls SerialPortHandler::initPort() with \p portName and \p settings. Emits the error if there is any.
 *
 * @param[in] portName Name of the port to change settings for.
 * @param[in] settings The settings struct.
 */
void SerialPortHandler::changePortSettings(QString portName, const SerialPortSettings settings) {
    QSerialPort::SerialPortError err = initPort(portName, settings);
    if (err != QSerialPort::NoError)
        emit sendCommandError(err);
}
