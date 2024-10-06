#pragma once

#include "HANetwork.h"
#include "Sensor.h"

class SensorManager;

class MqttDevice
{

public:

    MqttDevice(HANetwork *haNetwork, SensorManager *sensorManager);
    void registerDevice();
    void callback(const char* topic, byte* payload, unsigned int length);
    void updateSensors(Sensor *temperature, Sensor *tds, Sensor *ph, Sensor *level, bool fastRead);

protected:

    HANetwork *haNetwork;
    SensorManager *sensorManager;

    void initDiscoveryJsonDocument(JsonDocument &doc);

};
