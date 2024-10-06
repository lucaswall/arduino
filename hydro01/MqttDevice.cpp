
#include <ArduinoJson.h>

#include "MqttDevice.h"
#include "SensorManager.h"
#include "config.h"

typedef struct
{
    const char *name;
    const char *unique_id;
    const char *state_topic;
    const char *availability_topic;
    const char *unit_of_measurement;
    int suggested_display_precision;
    float multiplier;
    const char *icon;
} MqttSensor;

const MqttSensor sensorsDefinition[] = {
    {
        "Hydro01 Water Temperature",
        "hydro01_water_temperature",
        "hydro01/water_temperature/value",
        "hydro01/water_temperature/available",
        "°C",
        1,
        1,
        "mdi:thermometer"
    },
    {
        "Hydro01 Water PPM",
        "hydro01_water_ppm",
        "hydro01/water_ppm/value",
        "hydro01/water_ppm/available",
        "ppm",
        0,
        1,
        "mdi:beaker"
    },
    {
        "Hydro01 Water pH",
        "hydro01_water_ph",
        "hydro01/water_ph/value",
        "hydro01/water_ph/available",
        "pH",
        1,
        1,
        "mdi:ph"
    },
    {
        "Hydro01 Water Level",
        "hydro01_water_level",
        "hydro01/water_level/value",
        "hydro01/water_level/available",
        "%",
        0,
        100,
        "mdi:car-coolant-level"
    },
};

const char *action_topic = "hydro01/action";
const char *action_payload_fastread_on = "fastread_on";
const char *action_payload_fastread_off = "fastread_off";
const char *action_fastread_state = "hydro01/fastread";
const char *action_payload_on = "on";
const char *action_payload_off = "off";

MqttDevice::MqttDevice(HANetwork *haNetwork, SensorManager *sensorManager)
{
    this->haNetwork = haNetwork;
    this->sensorManager = sensorManager;
}

void MqttDevice::registerDevice()
{
    Serial.println(F("MqttDevice::registerDevice"));

    JsonDocument doc;

    initDiscoveryJsonDocument(doc);
    for (int i = 0; i < sizeof(sensorsDefinition) / sizeof(MqttSensor); i++)
    {
        doc["name"] = sensorsDefinition[i].name;
        doc["object_id"] = sensorsDefinition[i].unique_id;
        doc["unique_id"] = sensorsDefinition[i].unique_id;
        doc["state_topic"] = sensorsDefinition[i].state_topic;
        doc["availability_topic"] = sensorsDefinition[i].availability_topic;
        doc["force_update"] = true;
        doc["unit_of_measurement"] = sensorsDefinition[i].unit_of_measurement;
        doc["suggested_display_precision"] = sensorsDefinition[i].suggested_display_precision;
        doc["icon"] = sensorsDefinition[i].icon;

        char buf[200];
        sprintf(buf, "homeassistant/sensor/%s/config", sensorsDefinition[i].unique_id);
        haNetwork->mqttPublish(buf, doc, true);
    }

    initDiscoveryJsonDocument(doc);
    doc["name"] = "Hydro01 Fast Read";
    doc["object_id"] = "hydro01_fastread";
    doc["unique_id"] = "hydro01_fastread";
    doc["command_topic"] = action_topic;
    doc["payload_on"] = action_payload_fastread_on;
    doc["payload_off"] = action_payload_fastread_off;
    doc["state_topic"] = action_fastread_state;
    doc["state_on"] = action_payload_on;
    doc["state_off"] = action_payload_off;
    doc["icon"] = "mdi:fast-forward";
    haNetwork->mqttPublish("homeassistant/switch/hydro01_action/config", doc, true);

    haNetwork->mqttSubscribe(action_topic);

}

void MqttDevice::initDiscoveryJsonDocument(JsonDocument &doc)
{
    doc.clear();
    doc["schema"] = "json";
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = "hydro01";
    device["name"] = "Hydro01";
    device["model"] = "Wemos Hydroponic Sensors";
    device["manufacturer"] = "MLCM Tech";
}

void MqttDevice::callback(const char* topic, byte* payload, unsigned int length)
{
    if (strcmp(topic, action_topic) == 0)
    {
        if (strncasecmp((char*)payload, action_payload_fastread_on, length) == 0)
        {
            sensorManager->fastReadSensors(true);
            haNetwork->mqttPublish(action_fastread_state, action_payload_on, true);
        }
        else if (strncasecmp((char*)payload, action_payload_fastread_off, length) == 0)
        {
            sensorManager->fastReadSensors(false);
            haNetwork->mqttPublish(action_fastread_state, action_payload_off, true);
        }
    }
}

void MqttDevice::updateSensors(Sensor *temperature, Sensor *tds, Sensor *ph, Sensor *level, bool fastRead)
{
    Sensor *sensors[] = { temperature, tds, ph, level };
    for (int i = 0; i < sizeof(sensors) / sizeof(Sensor); i++)
    {
        Sensor *sensor = sensors[i];
        if (sensor->isError())
        {
            haNetwork->mqttPublish(sensorsDefinition[i].availability_topic, "offline", true);
            continue;
        }
        haNetwork->mqttPublish(sensorsDefinition[i].availability_topic, "online", true);
        char buf[20];
        sprintf(buf, "%.*f", sensorsDefinition[i].suggested_display_precision, sensor->getReading() * sensorsDefinition[i].multiplier);
        haNetwork->mqttPublish(sensorsDefinition[i].state_topic, buf, true);
    }
    haNetwork->mqttPublish(action_fastread_state, fastRead ? action_payload_on : action_payload_off, true);
}
