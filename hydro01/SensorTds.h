#pragma once

#include "SensorVoltage.h";

class SensorTds : public SensorVoltage
{
public:
    
    SensorTds(int pin, int readCount, int readDelay);

protected:

    virtual float calculateValue(SensorManager* sensorManager, float voltage) override;
    
};
