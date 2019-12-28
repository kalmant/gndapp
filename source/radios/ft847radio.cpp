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
    QByteArray temp;                            // set MODE to DIGI
    temp.append(static_cast<char>(0x0A));
    temp.append(static_cast<char>(0x00));
    temp.append(static_cast<char>(0x00));
    temp.append(static_cast<char>(0x00));
    temp.append(0x07);
    emit newCommand(temp);
}

/**
 * @brief Sets frequency to \p frequencyHz.
 * @param frequencyHz The new freuqncy as Hz
 */
void FT847Radio::setFrequency(unsigned long frequencyHz) {
    if (frequencyHz > 500000000) {
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
