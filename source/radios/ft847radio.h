#ifndef FT847RADIONEW_H
#define FT847RADIONEW_H

#include "radio.h"
#include <QString>

class FT847Radio : public Radio {
public:
    FT847Radio(PredicterController *predicter);

protected:
    void setPortSettingsBasedOnBaudRate(int baudRate) override;
    void initialization() override;
    void setFrequency(unsigned long frequencyHz) override;
    void turnOff() override;
    void turnOn() override;
};

#endif // FT847RADIO_H
