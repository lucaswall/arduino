#pragma once

#include "State.h"
#include "SwitchSensor.h"

class StateSwitchSensorOn : public State
{

public:
    
    StateSwitchSensorOn(SensorManager *sensorManager, const char *name, SwitchSensor *switchSensor);
    virtual void loop() override {}
    virtual void enter() override;

protected:

    SwitchSensor *switchSensor;

};
