#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

#include "Sensor.h"
#include "config.h"

class SensorTemperature : public Sensor
{

public:

    SensorTemperature(int pin);
    virtual ~SensorTemperature();
    virtual void init(SensorManager* sensorManager) override;
    virtual void loop() override {}
    virtual void startReading() override;
    virtual bool isReading() const override { return false; }
    virtual bool isError() const override { return lastTemperature == DEVICE_DISCONNECTED_C; }
    virtual float getReading() const override { return isError() ? RefTemperature : lastTemperature; }
    virtual float getReadProgress() const override { return 1.0; }

protected:

    OneWire *oneWire;
    DallasTemperature *sensor;
    float lastTemperature = DEVICE_DISCONNECTED_C;

};
