#pragma once

#include "config.h"

class SensorManager;

class State
{

public:

    State(SensorManager* sensorManager, State *nextState = nullptr);
    virtual void loop() = 0;
    virtual void enter() = 0;
    inline State *setNextState(State *nextState) { this->nextState = nextState; return nextState; }

protected:

    SensorManager* sensorManager;
    State *nextState = nullptr;

};
