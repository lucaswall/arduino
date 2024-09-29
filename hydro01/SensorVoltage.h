#pragma once

#include "Sensor.h"

class SensorVoltage : public Sensor
{

public:

    SensorVoltage(int pin, int readCount, int readDelay);
    virtual void init(SensorManager* sensorManager) override;
    virtual void loop() override;
    virtual void startReading() override;
    virtual bool isReading() const override { return pendingReadCount > 0; }
    virtual bool isError() const override { return false; }
    virtual float getReading() const override { return lastReading; }

protected:
    
    SensorManager* sensorManager;
    int pin = 0;
    int readCount = 30;
    int readDelay = 10;

    int pendingReadCount = 0;
    unsigned long nextReadTime = 0;
    float lastReading = 0;
    float pendingReading = 0;

    virtual float calculateValue(float voltage) = 0;

};
