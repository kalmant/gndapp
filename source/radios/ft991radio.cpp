#include "ft991radio.h"

FT991Radio::FT991Radio(PredicterController *predicter) : Radio(predicter, 125) {
    baseOffset_prot = -1500;
}

void FT991Radio::setPortSettingsBasedOnBaudRate(int baudRate) {
    if (baudRate == 4800) {
        portSettings_prot.BaudRate = QSerialPort::Baud4800;
    }
    else if (baudRate == 9600) {
        portSettings_prot.BaudRate = QSerialPort::Baud9600;
    }
    else {
        portSettings_prot.BaudRate = QSerialPort::Baud19200;
    }
    portSettings_prot.FlowControl = QSerialPort::NoFlowControl;
    portSettings_prot.Parity = QSerialPort::NoParity;
    if (portSettings_prot.BaudRate == QSerialPort::Baud4800) {
        portSettings_prot.StopBits = QSerialPort::TwoStop;
    }
    else {
        portSettings_prot.StopBits = QSerialPort::OneStop;
    }
}

/**
 * @brief FT991 requires no additional operations during starting up
 */
void FT991Radio::initialization() {
    emit newCommand(QString("MD02;").toLocal8Bit());     // set MODE to USB
    emit newCommand(QString("RC;").toLocal8Bit());       // clear CLAR
}

/**
 * @brief Sets VFO-A frequency to \p frequencyHz
 * @param frequencyHz Frequency as Hz
 */
void FT991Radio::setFrequency(unsigned long frequencyHz) {
    emit newCommand(QString("FA%1;").arg(frequencyHz, 9, 10, QChar('0')).toLocal8Bit());
}

/**
 * @brief Does not turn the radio off, since there is no way to turn it back on.
 */
void FT991Radio::turnOff() {
}

/**
 * @brief There is no way to turn the radio on, so does nothing.
 */
void FT991Radio::turnOn() {
}
