#pragma once

#include "State.h"

class StateReadSensor : public State
{

public:
    
    StateReadSensor(SensorManager* sensorManager, SensorType sensorType, State *nextState);
    virtual void loop() override;
    virtual void enter() override;

protected:

    SensorType sensorType;
    State *nextState = nullptr;

};
