#ifndef TS2000RADIONEW_H
#define TS2000RADIONEW_H

#include "radio.h"

class TS2000Radio : public Radio {
public:
    TS2000Radio(PredicterController *predicter);

protected:
    void setPortSettingsBasedOnBaudRate(int baudRate) override;
    void initialization() override;
    void setFrequency(unsigned long frequencyHz) override;
    void turnOff() override;
    void turnOn() override;
};

#endif // TS2000RADIO_H
