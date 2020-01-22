#include "smogradio.h"

SMOGRadio::SMOGRadio(PredicterController *predicter, PacketDecoder *pd) : Radio(predicter, 100, true) {
    baseOffset_prot = 0;
    dataRateBPS_priv = 1250;
    packetLengthBytes_priv = 70;
    QObject::connect(sph_prot.data(), &SerialPortHandler::dataRead, this, &SMOGRadio::dataFromSerialPortSlot);
    QObject::connect(pd, &PacketDecoder::newPacketLength, this, &SMOGRadio::newPacketLengthSlot);
    QObject::connect(pd, &PacketDecoder::newDataRate, this, &SMOGRadio::newDataRateSlot);
    QObject::connect(this, &SMOGRadio::dataReady, pd, &PacketDecoder::decodablePacketReceivedWithRssi);
}

void SMOGRadio::set5VOut(bool value) {
    this->fiveVoltsOut = value;
}

void SMOGRadio::set5VOutInRadio() {
    if (this->fiveVoltsOut) {
        enable5VOutInRadio();
    }
    else {
        disable5VOutInRadio();
    }
}

void SMOGRadio::disable5VOutInRadio() {
    emit newCommand(QString("v0\n").toLocal8Bit());
}

void SMOGRadio::enable5VOutInRadio() {
    emit newCommand(QString("v1\n").toLocal8Bit());
}

void SMOGRadio::setPortSettingsBasedOnBaudRate(int baudRate) {
    if (baudRate != 115200) {
        qWarning() << "Invalid baud rate for SMOG receiver, should be 115200";
    }
    portSettings_prot.BaudRate = QSerialPort::Baud115200;
    portSettings_prot.FlowControl = QSerialPort::NoFlowControl;
    portSettings_prot.Parity = QSerialPort::NoParity;
    portSettings_prot.StopBits = QSerialPort::OneStop;
}

void SMOGRadio::initialization() {
    setFrequency(baseFrequency_prot + offsetHz_prot);
    setPacketLength();
    setDatarate();
}

void SMOGRadio::setFrequency(unsigned long frequencyHz) {
    emit newCommand(QString("f%1\n").arg(frequencyHz, 9, 10, QChar('0')).toLocal8Bit());
}

void SMOGRadio::turnOff() {
    disable5VOutInRadio();
}

void SMOGRadio::turnOn() {
    set5VOutInRadio();
}

void SMOGRadio::setDatarate() {
    emit newCommand(QString("d%1\n").arg(dataRateBPS_priv, 6, 10, QChar('0')).toLocal8Bit());
}

void SMOGRadio::setPacketLength() {
    emit newCommand(QString("l%1\n").arg(packetLengthBytes_priv, 6, 10, QChar('0')).toLocal8Bit());
}

/**
 * @brief Deals with the data read from the serial port
 * @param data Data read from the serial port as a QString
 */
void SMOGRadio::dataFromSerialPortSlot(QString data) {
    if (data.length() < 20) {
        qInfo() << "SMOGRADIO command reply: " + data;
        return;
    }
    qInfo() << "Data from serial port in SMOGRADIO, length:" << data.length() << "dBm:" << data.right(6).left(4);
    qInfo() << "Data received from SMOGRADIO:" << data;
    QDateTime timestamp = QDateTime::currentDateTimeUtc();
    QString source = QString("SMOGRADIO ").append(QString::number(dataRateBPS_priv)).append(" BPS");
    emit dataReady(timestamp, source, data.left(data.length() - 7), data.right(6).left(4).toInt());
}

/**
 * @brief Sets packet length to \p newPacketLengthBytes. Calls SMOGRadioReceiver::setPacketLength() if necessary.
 * @param newPacketLengthBytes New packet length.
 */
void SMOGRadio::newPacketLengthSlot(unsigned int newPacketLengthBytes) {
    if (newPacketLengthBytes != packetLengthBytes_priv) {
        packetLengthBytes_priv = newPacketLengthBytes;
        setPacketLength();
    }
}

/**
 * @brief Changes the datarate to \p newDataRateBPS . Calls SMOGRadioReceiver::setDatarate() if necessary.
 * @param newDataRateBPS New data rate
 */
void SMOGRadio::newDataRateSlot(unsigned int newDataRateBPS) {
    if (newDataRateBPS != dataRateBPS_priv) {
        dataRateBPS_priv = newDataRateBPS;
        setDatarate();
    }
}
