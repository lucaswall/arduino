#pragma once

#include "HANetwork.h"
#include "Sensor.h"

class MqttDevice
{

public:

    MqttDevice(HANetwork *haNetwork);
    void registerDevice();
    void updateSensors(Sensor *temperature, Sensor *tds, Sensor *ph, Sensor *level);

protected:

    HANetwork *haNetwork;

};
