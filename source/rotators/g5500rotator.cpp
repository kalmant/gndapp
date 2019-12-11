#include "g5500rotator.h"

G5500Rotator::G5500Rotator(PredicterController *predicter) : Rotator(predicter, 10) {
}

void G5500Rotator::setAZEL(unsigned int azimuth, unsigned int elevation) {
    emit newCommand(
        QString("W%1 %2\r\n").arg(azimuth, 3, 10, QChar('0')).arg(elevation, 3, 10, QChar('0')).toLocal8Bit());
}

void G5500Rotator::setPortSettingsBasedOnBaudRate(int baudRate) {
    switch (baudRate) {
    case 1200:
        portSettings_prot.BaudRate = QSerialPort::Baud1200;
        break;
    case 2400:
        portSettings_prot.BaudRate = QSerialPort::Baud2400;
        break;
    case 4800:
        portSettings_prot.BaudRate = QSerialPort::Baud4800;
        break;
    case 9600:
        portSettings_prot.BaudRate = QSerialPort::Baud9600;
        break;
    default:
        portSettings_prot.BaudRate = QSerialPort::Baud9600;
        break;
    }
    portSettings_prot.FlowControl = QSerialPort::NoFlowControl;
    portSettings_prot.Parity = QSerialPort::NoParity;
    portSettings_prot.StopBits = QSerialPort::OneStop;
}

void G5500Rotator::initialization() {
    // does nothing
}
