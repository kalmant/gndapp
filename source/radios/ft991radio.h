#ifndef FT991RADIONEW_H
#define FT991RADIONEW_H

#include "radio.h"
#include <QString>

class FT991Radio : public Radio {
public:
    FT991Radio(PredicterController *predicter);

protected:
    void setPortSettingsBasedOnBaudRate(int baudRate) override;
    void initialization() override;
    void setFrequency(unsigned long frequencyHz) override;
    void turnOff() override;
    void turnOn() override;
};

#endif // FT991RADIO_H
