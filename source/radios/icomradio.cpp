#include "icomradio.h"

ICOMRadio::ICOMRadio(PredicterController *predicter) : Radio(predicter, 300) {
    baseOffset_prot = -1500;
}

void ICOMRadio::setPortSettingsBasedOnBaudRate(int baudRate) {
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
    portSettings_prot.StopBits = QSerialPort::OneStop;
}

/**
 * @brief Sets mode and other settings for the radio.
 */
void ICOMRadio::initialization() {
    QByteArray temp; // set MODE to USB
    temp.append(static_cast<char>(0xFE));
    temp.append(static_cast<char>(0xFE));
    temp.append(static_cast<char>(0x00));
    temp.append(static_cast<char>(0xE0));
    temp.append(0x06);
    temp.append(0x01);
    temp.append(static_cast<char>(0xFD));
    emit newCommand(temp);
}

/**
 * @brief Sets frequency to \p frequencyHz.
 * @param frequencyHz The new freuqncy as Hz
 */
void ICOMRadio::setFrequency(unsigned long frequencyHz) {
    QByteArray temp;
    unsigned long truncated = frequencyHz;
    unsigned char fifth = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char fourth = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char third = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char second = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char first = static_cast<unsigned char>(truncated % 100);
    temp.append(static_cast<char>(0xFE));
    temp.append(static_cast<char>(0xFE));
    temp.append(static_cast<char>(0x00));
    temp.append(static_cast<char>(0xE0));
    temp.append(0x05);
    temp.append((fifth / 10) * 16 + (fifth % 10));
    temp.append((fourth / 10) * 16 + (fourth % 10));
    temp.append((third / 10) * 16 + (third % 10));
    temp.append((second / 10) * 16 + (second % 10));
    temp.append((first / 10) * 16 + (first % 10));
    temp.append(static_cast<char>(0xFD));
    emit newCommand(temp);
}

void ICOMRadio::turnOn() {
}

void ICOMRadio::turnOff() {
}
