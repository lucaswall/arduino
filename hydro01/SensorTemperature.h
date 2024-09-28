#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

#include "Sensor.h";

class SensorTemperature : Sensor
{

public:

    SensorTemperature(int pin);
    virtual ~SensorTemperature();
    virtual void init() override;
    virtual float read(SensorManager* sensorManager) override;
    virtual bool haveReading() const override { return lastTemperature != DEVICE_DISCONNECTED_C; }

protected:

    OneWire *oneWire;
    DallasTemperature *sensor;
    float lastTemperature = DEVICE_DISCONNECTED_C;

};
