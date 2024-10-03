
#include <ArduinoJson.h>

#include "MqttDevice.h"

void MqttDevice::registerDevice()
{
    Serial.println(F("MqttDevice::registerDevice"));

    StaticJsonDocument<512> doc;
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = "hydro01";
    device["name"] = "Hydroponic Sensors";
    device["model"] = "Wemos Hydroponic Sensors";
    device["manufacturer"] = "MLCM Tech";

}
