#pragma once

class SensorManager;

class Sensor
{

public:

    virtual void init(SensorManager* sensorManager) = 0;
    virtual void loop() = 0;
    virtual void startReading() = 0;
    virtual bool isReading() const = 0;
    virtual bool isError() const = 0;
    virtual float getReading() const = 0;
    virtual float getReadProgress() const = 0;

};
