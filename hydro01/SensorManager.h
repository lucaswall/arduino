#pragma once

#include "Sensor.h"

class State;

class SensorManager
{

public:

    void loop();
    void setState(State* state);

    inline void setTemperature(float temperature) { this->temperature = temperature; }
    inline float getTemperature() const { return temperature; }

protected:

    State* currentState = nullptr;
    float temperature = 25;

};
