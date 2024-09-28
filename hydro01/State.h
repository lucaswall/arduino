#pragma one

#include "Arduino.h";

class SensorReader;

class State
{

public:

    State(SensorReader* sensorReader);
    virtual void loop() = 0;
    virtual void enter() = 0;

protected:

    SensorReader* sensorReader;

};
