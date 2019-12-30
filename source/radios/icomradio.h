#ifndef ICOMRADIONEW_H
#define ICOMRADIONEW_H

#include "radio.h"
#include <QString>

class ICOMRadio : public Radio {
public:
    ICOMRadio(PredicterController *predicter);

protected:
    void setPortSettingsBasedOnBaudRate(int baudRate) override;
    void initialization() override;
    void setFrequency(unsigned long frequencyHz) override;
    void turnOff() override;
    void turnOn() override;
};

#endif // ICOMRADIO_H
