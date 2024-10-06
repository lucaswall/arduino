#pragma once

#include "State.h"
#include "SwitchSensor.h"

class StateSwitchSensorOff : public State
{

public:
    
    StateSwitchSensorOff(SensorManager *sensorManager, SwitchSensor *switchSensor, State *nextState = nullptr);
    virtual void loop() override {}
    virtual void enter() override;

protected:

    SwitchSensor *switchSensor;

};
