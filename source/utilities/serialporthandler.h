#ifndef SERIALPORTHANDLER_H
#define SERIALPORTHANDLER_H

#include <QDebug>
#include <QList>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QTimer>

/**
 * @brief Struct with common settings for QSerialPort instances
 *
 * A struct to keep the four commonly used settings easy to maintain when handling QSerialPort instances.
 *
 */
struct SerialPortSettings {
    QSerialPort::BaudRate BaudRate;       //!< Baud Rate used during communication
    QSerialPort::Parity Parity;           //!< Parity used during communication
    QSerialPort::StopBits StopBits;       //!< Stops Bits used during communication
    QSerialPort::FlowControl FlowControl; //!< Flow Control used during communication
};

/**
 * @brief A class that provides a very easy to use interface for serial port communcation with a message queue.
 */
class SerialPortHandler : public QObject {
    Q_OBJECT
public:
    SerialPortHandler(int msecsBetweenCommands_priv = 225, bool readFromDevice = false, QObject *parent = 0);

    const QList<QSerialPortInfo> getPorts();

private:
    QSerialPort port_priv;                 //!< QSerialPort object to the port that this object is utilizing
    QByteArray buffer_priv;                //!< QByteArray that buffers input until end of line is found
    QTimer timer_priv;                     //!< QTimer object used for the timing of the commands
    int msecsBetweenCommands_priv;         //!< Number of milliseconds between commands sent
    QList<QByteArray> commandsToSend_priv; //!< A FIFO list of commands to send
    bool readFromDevice_priv; //!< True if the port should be opened in ReadWrite mode and it emits data read

    QSerialPort::SerialPortError sendNextCommand();
    QSerialPort::SerialPortError initPort(QString portName, const SerialPortSettings settings);

private slots:
    void handleReadyRead();

public slots:
    void newCommandSlot(const QByteArray &command);
    void changePortSettings(QString portName, const SerialPortSettings settings);
    void clearCommandList();
    void closePort();

signals:
    /**
     * @brief Signal used to communicate the errors that occurred.
     */
    void sendCommandError(QSerialPort::SerialPortError);

    /**
     * @brief Emits the data read from the serial port
     * @param data The data read from the serial port
     */
    void dataRead(QString data);
};

#endif // SERIALPORTHANDLER_H
