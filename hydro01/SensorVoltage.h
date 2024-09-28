#pragma once

#include "Sensor.h";

class SensorVoltage : Sensor
{

public:

    SensorVoltage(int pin, int readCount, int readDelay);
    virtual void init() override;
    virtual float read(SensorManager* sensorManager) override;

protected:
    
    int pin = 0;
    int readCount = 30;
    int readDelay = 10;

    virtual float calculateValue(SensorManager* sensorManager, float voltage) = 0;

};
