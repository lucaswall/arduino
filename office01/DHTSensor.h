#pragma once

#include <DHT.h>

#include "HANetwork.h"

class DHTSensor
{

public:
    DHTSensor(HANetwork &haNetwork, unsigned long readFreqInit, uint8_t inputPin, uint8_t sensorType, const char *deviceId, const char *roomName);
    void setup();
    void loop();

private:
    unsigned long readFreq;
    static const unsigned long readInitPause = 5000;
    HANetwork &haNetwork;
    DHT dht;
    unsigned long nextReadTime = 0;
    String deviceId;
    String roomName;
    String topicAvailable;
    String topicTemperature;
    String topicHumidity;
    String topicHeatIndex;
    String topicUptime;

};
