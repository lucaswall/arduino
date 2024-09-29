#pragma once

#include "SensorVoltage.h"

class SensorTds : public SensorVoltage
{

public:

    SensorTds(int pin, int readCount, int readDelay) : SensorVoltage(pin, readCount, readDelay) {}

protected:

    virtual float calculateValue(float voltage) override;
    
};
