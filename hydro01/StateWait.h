#pragma once

#include "State.h"

class StateWait : public State
{

public:

    StateWait(SensorManager* sensorManager, uint32_t waitTime, State *nextState = nullptr);
    virtual void loop() override;
    virtual void enter() override;

protected:

    uint32_t waitTime = 0;
    uint32_t exitTime = 0;

};
