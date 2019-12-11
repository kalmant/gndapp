#ifndef SMOGRADIORECEIVER_H
#define SMOGRADIORECEIVER_H

#include "../packet/packetdecoder.h"
#include "radio.h"

class SMOGRadio : public Radio {
    Q_OBJECT
public:
    SMOGRadio(PredicterController *predicter, PacketDecoder *pd);

    Q_INVOKABLE void set5VOut(bool value);

private:
    unsigned int dataRateBPS_priv = 0;
    unsigned int packetLengthBytes_priv = 0;
    bool fiveVoltsOut = false;

    void set5VOutInRadio();
    void disable5VOutInRadio();
    void enable5VOutInRadio();

protected:
    void setPortSettingsBasedOnBaudRate(int baudRate) override;
    void initialization() override;
    void setFrequency(unsigned long frequencyHz) override;
    void turnOff() override;
    void turnOn() override;
    void setDatarate();
    void setPacketLength();

signals:
    /**
     * @brief Signal that is emitted when a packet has been detected.
     * @param[in] timestamp QString that represents the current time in UTC.
     * @param source The source of the packet
     * @param packetUpperHexString QString that represents the data contained in the packet
     * @param rssi The RSSI that the packet was received with
     */
    void dataReady(QDateTime timestamp, QString source, QString packetUpperHexString, int rssi);

private slots:
    void dataFromSerialPortSlot(QString data);
    void newDataRateSlot(unsigned int newDataRateBPS);
    void newPacketLengthSlot(unsigned int newPacketLengthBytes);
};

#endif // SMOGRADIO_H
