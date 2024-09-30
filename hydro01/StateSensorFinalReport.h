#pragma once

#include "State.h"

class SensorManager;

class StateSensorFinalReport : public State
{

public:
    
    StateSensorFinalReport(SensorManager* sensorManager, State *nextState);
    virtual void loop() override {}
    virtual void enter() override;

protected:

    State *nextState = nullptr;

};
