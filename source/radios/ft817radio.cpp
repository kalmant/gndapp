#include "ft817radio.h"
#include <cmath>

FT817Radio::FT817Radio(PredicterController *predicter) : Radio(predicter, 300) {
    frequencyStep_priv = 5000;
    baseOffset_prot = -1500;
}

/**
 * @brief Creates and returns a QByteArray from five chars received as parameters.
 * @param[in] one First character of the QByteArray.
 * @param[in] two Second character of the QByteArray.
 * @param[in] three Third character of the QByteArray.
 * @param[in] four Fourth character of the QByteArray.
 * @param[in] five Fifth character of the QByteArray.
 * @return The QByteArray that is created by concatenating the chars.
 */
const QByteArray FT817Radio::command5BytesFrom5Chars(
    const char one, const char two, const char three, const char four, const char five) {
    QByteArray temp;
    temp.append(one);
    temp.append(two);
    temp.append(three);
    temp.append(four);
    temp.append(five);
    return temp;
}

void FT817Radio::setPortSettingsBasedOnBaudRate(int baudRate) {
    if (baudRate == 4800) {
        portSettings_prot.BaudRate = QSerialPort::Baud4800;
    }
    else {
        portSettings_prot.BaudRate = QSerialPort::Baud9600;
    }
    portSettings_prot.FlowControl = QSerialPort::NoFlowControl;
    portSettings_prot.Parity = QSerialPort::NoParity;
    portSettings_prot.StopBits = QSerialPort::TwoStop;
}

/**
 * @brief Sets mode and other settings for the radio.
 */
void FT817Radio::initialization() {
    emit newCommand(command5BytesFrom5Chars(0x0A, 0x00, 0x00, 0x00, 0x07));      // DIG MODE
    setFREQto(baseFrequency_prot + static_cast<unsigned long>(baseOffset_prot)); // setting freq. to baseFrequency_prot
    emit newCommand(command5BytesFrom5Chars(0x00, 0x00, 0x00, 0x00, static_cast<char>(0xF5))); // setting CLAR to +0
    emit newCommand(command5BytesFrom5Chars(0x00, 0x00, 0x00, 0x00, 0x05));                    // turning on CLAR
}

/**
 * @brief Sets CLAR to \p frequencyHz if its absolute value is smaller than 10kHZ
 * @param frequencyHz The new CLAR freq. as Hz
 */
void FT817Radio::setCLARto(int frequencyHz) {
    if (frequencyHz > 9999 || frequencyHz < -9999) {
        qWarning() << "Invalid CLAR value, should be less than 10kHz";
        return;
    }

    char sign = frequencyHz > 0 ? 0x00 : 0x01; // CLAR sign
    int truncated = 10 * std::floor(frequencyHz / 10.0 + 0.5);     // CLAR supports 10 Hz accuracy
    qInfo() << "CLAR set to " << frequencyHz << ", rounded to " << truncated << " Hz";

    unsigned char first = static_cast<unsigned char>(truncated / 100);
    unsigned char second = static_cast<unsigned char>(truncated % 100);
    emit newCommand(command5BytesFrom5Chars(
        sign, 0x00, (first / 10) * 16 + (first % 10), (second / 10) * 16 + (second % 10), static_cast<char>(0xF5)));
}

/**
 * @brief Sets frequency to \p frequencyHz if it is smaller than 500Mhz. Modifies \p currentFrequency_priv accordingly.
 * @param frequencyHz The new freuqncy as Hz
 */
void FT817Radio::setFREQto(unsigned long frequencyHz) {
    if (frequencyHz > 500000000) {
        qWarning() << "Invalid frequency, it should be less than 500 MHz";
        return;
    }

    unsigned long truncated = frequencyHz / 10;
    unsigned char fourth = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char third = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char second = static_cast<unsigned char>(truncated % 100);
    truncated /= 100;
    unsigned char first = static_cast<unsigned char>(truncated % 100);
    emit newCommand(command5BytesFrom5Chars((first / 10) * 16 + (first % 10),
        (second / 10) * 16 + (second % 10),
        (third / 10) * 16 + (third % 10),
        (fourth / 10) * 16 + (fourth % 10),
        0x01));
    currentFrequency_priv = frequencyHz;
}

/**
 * @brief Changes the frequency of the radio.
 *
 * Utilizes CLAR to set the frequency of the radio due to problems with the radio.
 * Whenever the difference between \p frequencyHz and \p currentFrequency_priv is bigger than \p frequencySte_priv \p
 * currentFrequency_priv is moved so that \p frequencyHz is the edge of the new CLAR interval.
 *
 * @param frequencyHz New frequency that the radio will be set to.
 */
void FT817Radio::setFrequency(unsigned long frequencyHz) {
    if (static_cast<unsigned int>(abs(static_cast<long>(frequencyHz) - static_cast<long>(currentFrequency_priv))) >
        frequencyStep_priv) {
        // Setting new frequency
        unsigned long baseFreq;
        baseFreq = (frequencyHz > currentFrequency_priv) ?
                       (frequencyHz + frequencyStep_priv) :
                       (frequencyHz - frequencyStep_priv); // The new freq. will be the edge of the new freq. interval
        setFREQto(baseFreq);
    }
    setCLARto(static_cast<int>(frequencyHz - currentFrequency_priv));
}

void FT817Radio::turnOn() {
    if (turnOnOff_prot) {
        emit newCommand(command5BytesFrom5Chars(0x00, 0x00, 0x00, 0x00, 0x00)); // empty bytes before turning on
        emit newCommand(command5BytesFrom5Chars(0x00, 0x00, 0x00, 0x00, 0x0F)); // power on
    }
}

void FT817Radio::turnOff() {
    if (turnOnOff_prot) {
        emit newCommand(command5BytesFrom5Chars(0x00, 0x00, 0x00, 0x00, static_cast<char>(0x8F))); // turning off
    }
}
