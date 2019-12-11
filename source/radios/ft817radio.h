#ifndef FT817RADIONEW_H
#define FT817RADIONEW_H

#include "radio.h"
#include <QString>

class FT817Radio : public Radio {
    Q_OBJECT
public:
    FT817Radio(PredicterController *predicter);

protected:
    void setPortSettingsBasedOnBaudRate(int baudRate) override;
    void initialization() override;
    void setFrequency(unsigned long frequencyHz) override;
    void turnOff() override;
    void turnOn() override;

private:
    unsigned int frequencyStep_priv;
    unsigned long currentFrequency_priv;

    const QByteArray command5BytesFrom5Chars(
        const char one, const char two, const char three, const char four, const char five);
    void setCLARto(int frequencyHz);
    void setFREQto(unsigned long frequencyHz);
};

#endif // FT817RADIO_H
