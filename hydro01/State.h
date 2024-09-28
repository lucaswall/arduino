#pragma one

#include "Arduino.h";

class SensorManager;

class State
{

public:

    State(SensorManager* sensorManager);
    virtual void loop() = 0;
    virtual void enter() = 0;

protected:

    SensorManager* sensorManager;

};
