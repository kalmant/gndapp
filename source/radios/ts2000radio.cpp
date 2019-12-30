#include "ts2000radio.h"

TS2000Radio::TS2000Radio(PredicterController *predicter) : Radio(predicter, 125) {
    baseOffset_prot = -1500;
}

void TS2000Radio::setPortSettingsBasedOnBaudRate(int baudRate) {
    if (baudRate == 4800) {
        portSettings_prot.BaudRate = QSerialPort::Baud4800;
    }
    else if (baudRate == 9600) {
        portSettings_prot.BaudRate = QSerialPort::Baud9600;
    }
    else if (baudRate == 19200) {
        portSettings_prot.BaudRate = QSerialPort::Baud19200;
    }
    else {
        portSettings_prot.BaudRate = QSerialPort::Baud38400;
    }
    portSettings_prot.FlowControl = QSerialPort::HardwareControl;
    portSettings_prot.Parity = QSerialPort::NoParity;
    if (portSettings_prot.BaudRate == QSerialPort::Baud4800) {
        portSettings_prot.StopBits = QSerialPort::TwoStop;
    }
    else {
        portSettings_prot.StopBits = QSerialPort::OneStop;
    }
}

/**
 * @brief TS2000 requires no additional operations during starting up
 */
void TS2000Radio::initialization() {
    emit newCommand(QString("MD2;").toLocal8Bit()); // Based on om3bc's input
}

/**
 * @brief Sets VFO-A frequency to \p frequencyHz
 * @param frequencyHz Frequency as Hz
 */
void TS2000Radio::setFrequency(unsigned long frequencyHz) {
    emit newCommand(QString("FA%1;").arg(frequencyHz, 11, 10, QChar('0')).toLocal8Bit());
}

/**
 * @brief Does not turn the radio off, since there is no way to turn it back on.
 */
void TS2000Radio::turnOff() {
    // emit newCommand(QString("PS0;").toLocal8Bit());
}

/**
 * @brief There is no way to turn the radio on, so does nothing.
 */
void TS2000Radio::turnOn() {
}
