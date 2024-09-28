#pragma once

class SensorManager;

class Sensor {

public:

    virtual void init() = 0;
    virtual float read(SensorManager* sensorManager) = 0;

};
