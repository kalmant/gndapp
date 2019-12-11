#ifndef G5000ROTATORNEW_H
#define G5000ROTATORNEW_H

#include "rotator.h"

class G5500Rotator : public Rotator {
public:
    G5500Rotator(PredicterController *predicter);

protected:
    void setAZEL(unsigned int azimuth, unsigned int elevation) override;
    void setPortSettingsBasedOnBaudRate(int baudRate) override;
    void initialization() override;
};

#endif // G5000ROTATOR_H
