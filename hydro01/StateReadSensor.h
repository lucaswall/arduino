#pragma once

#include "State.h"

class StateReadSensor : public State
{

public:
    
    StateReadSensor(SensorManager* sensorManager, const char *name, SensorType sensorType);
    virtual void loop() override;
    virtual void enter() override;

protected:

    SensorType sensorType;

};
