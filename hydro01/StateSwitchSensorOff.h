#pragma once

#include "State.h"
#include "SwitchSensor.h"

class StateSwitchSensorOff : public State
{

public:
    
    StateSwitchSensorOff(SensorManager *sensorManager, const char *name, SwitchSensor *switchSensor);
    virtual void loop() override {}
    virtual void enter() override;

protected:

    SwitchSensor *switchSensor;

};
