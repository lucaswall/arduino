#pragma once

#include "SensorVoltage.h"

class SensorLevel : public SensorVoltage
{

public:

    SensorLevel(int pin, int readCount, int readDelay) : SensorVoltage(pin, readCount, readDelay) {}

protected:

    virtual float calculateValue(float voltage) override;

};
