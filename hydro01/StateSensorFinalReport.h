#pragma once

#include "State.h"

class SensorManager;
class MqttDevice;

class StateSensorFinalReport : public State
{

public:
    
    StateSensorFinalReport(SensorManager* sensorManager, MqttDevice *mqttDevice, State *nextState);
    virtual void loop() override {}
    virtual void enter() override;

protected:

    MqttDevice *mqttDevice;
    State *nextState = nullptr;

};
