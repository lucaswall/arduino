#pragma once

#include "State.h"

class SensorManager;
class MqttDevice;

class StateSensorFinalReport : public State
{

public:
    
    StateSensorFinalReport(SensorManager* sensorManager, const char *name, MqttDevice *mqttDevice);
    virtual void loop() override {}
    virtual void enter() override;

protected:

    MqttDevice *mqttDevice;

};
