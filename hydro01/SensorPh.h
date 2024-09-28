#pragma once

#include "SensorVoltage.h";

class SensorPh : public SensorVoltage
{

public:
    
    SensorPh(int pin, int readCount, int readDelay);

protected:

    virtual float calculateValue(SensorManager* sensorManager, float voltage) override;

};
