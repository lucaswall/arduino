#pragma once

#include "config.h"

class SensorManager;

class State
{

public:

    State(SensorManager* sensorManager, const char *name);
    virtual void loop() = 0;
    virtual void enter() = 0;
    const char *getStatus() { return status.c_str(); }
    inline State *setNextState(State *nextState) { this->nextState = nextState; return nextState; }

protected:

    SensorManager* sensorManager;
    State *nextState = nullptr;
    String name;
    String status;

};
