#pragma once

#include "SensorVoltage.h"

class SensorPh : public SensorVoltage
{

public:

    SensorPh(int pin, int readCount, int readDelay) : SensorVoltage(pin, readCount, readDelay) {}

protected:

    virtual float calculateValue(float voltage) override;

};
