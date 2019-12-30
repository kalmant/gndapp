#include "ft847radio.h"

FT847Radio::FT847Radio(PredicterController *predicter) : Radio(predicter, 300) {
    baseOffset_prot = -1500;
}

void FT847Radio::setPortSettingsBasedOnBaudRate(int baudRate) {
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
    portSettings_prot.FlowControl = QSerialPort::NoFlowControl;
    portSettings_prot.Parity = QSerialPort::NoParity;
    portSettings_prot.StopBits = QSerialPort::TwoStop;
}

/**
 * @brief Sets mode and other settings for the radio.
 */
void FT847Radio::initialization() {
    QByteArray caton; // set CAT ON
    caton.append(static_cast<char>(0x00));
    caton.append(static_cast<char>(0x00));
    caton.append(static_cast<char>(0x00));
    caton.append(static_cast<char>(0x00));
    caton.append(static_cast<char>(0x00));
    emit newCommand(caton);
    QByteArray setusb; // set MODE to USB
    setusb.append(static_cast<char>(0x0A));
    setusb.append(static_cast<char>(0x00));
    setusb.append(static_cast<char>(0x00));
    setusb.append(static_cast<char>(0x00));
    setusb.append(0x07);
    emit newCommand(setusb);
}

/**
 * @brief Sets frequency to \p frequencyHz.
 * @param frequencyHz The new freuqncy as Hz
 */
void FT847Radio::setFrequency(unsigned long frequencyHz) {
    if (frequencyHz >= 500000000) {
        qWarning() << "Invalid frequency, it should be less than 500 MHz";
        return;
    }

    QByteArray temp;
    unsigned long truncated = frequencyHz / 10;
    unsigned char fourth = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char third = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char second = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char first = static_cast<unsigned char>(truncated % 100);
    temp.append((first / 10) * 16 + (first % 10));
    temp.append((second / 10) * 16 + (second % 10));
    temp.append((third / 10) * 16 + (third % 10));
    temp.append((fourth / 10) * 16 + (fourth % 10));
    temp.append(static_cast<char>(0x01));
    emit newCommand(temp);
}

void FT847Radio::turnOn() {
}

void FT847Radio::turnOff() {
}
