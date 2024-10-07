#pragma once

#include "State.h"

class StateWait : public State
{

public:

    StateWait(SensorManager* sensorManager, const char *name, uint32_t waitTime);
    virtual void loop() override;
    virtual void enter() override;

protected:

    uint32_t waitTime = 0;
    uint32_t exitTime = 0;

};
