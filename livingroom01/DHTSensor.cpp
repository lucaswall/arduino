
#include "DHTSensor.h"

DHTSensor::DHTSensor(HANetwork &haNetworkInit, unsigned long readFreqInit, uint8_t inputPin, uint8_t sensorType, const char *deviceIdInit, const char *roomNameInit)
    : haNetwork(haNetworkInit), dht(inputPin, sensorType)
{
    readFreq = readFreqInit;
    deviceId = deviceIdInit;
    roomName = roomNameInit;
    topicAvailable = deviceId + "/dht/available";
    topicTemperature = deviceId + "/dht/temperature";
    topicHumidity = deviceId + "/dht/humidity";
    topicHeatIndex = deviceId + "/dht/heatindex";
    topicUptime = deviceId + "/dht/uptime";
}

void DHTSensor::setup()
{

    Serial.print("topicAvailiable = "); Serial.println(topicAvailable);
    Serial.print("topicTemperature = "); Serial.println(topicTemperature);
    Serial.print("topicHumidity = "); Serial.println(topicHumidity);
    Serial.print("topicHeatIndex = "); Serial.println(topicHeatIndex);
    Serial.print("topicUptime = "); Serial.println(topicUptime);

    dht.begin();
    dht.readHumidity(true);
    nextReadTime = millis() + readInitPause;

    StaticJsonDocument<512> doc;
    char buf[200];

    // device information
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = deviceId;
    device["name"] = (sprintf(buf, "%s Sensors U01", roomName.c_str()), buf);
    device["model"] = "Wemos DHT Sensor";
    device["manufacturer"] = "MLCM Tech";
    device["suggested_area"] = roomName;

    doc["name"] = (sprintf(buf, "%s Temperature", roomName.c_str()), buf);
    doc["unique_id"] = (sprintf(buf, "%s.temperature", deviceId.c_str()), buf);
    doc["device_class"] = "temperature";
    doc["state_topic"] = topicTemperature.c_str();
    doc["availability_topic"] = topicAvailable.c_str();
    doc["force_update"] = true;
    doc["unit_of_measurement"] = "°C";
    doc["suggested_display_precision"] = 1;
    sprintf(buf, "homeassistant/sensor/%s_temperature/config", deviceId.c_str());
    haNetwork.mqttConfig(buf, doc);

    doc["name"] = (sprintf(buf, "%s Humidity", roomName.c_str()), buf);
    doc["unique_id"] = (sprintf(buf, "%s.humidity", deviceId.c_str()), buf);
    doc["device_class"] = "humidity";
    doc["state_topic"] = topicHumidity.c_str();
    doc["availability_topic"] = topicAvailable.c_str();
    doc["force_update"] = true;
    doc["unit_of_measurement"] = "%";
    doc["suggested_display_precision"] = 0;
    sprintf(buf, "homeassistant/sensor/%s_humidity/config", deviceId.c_str());
    haNetwork.mqttConfig(buf, doc);

    doc["name"] = (sprintf(buf, "%s Heat Index", roomName.c_str()), buf);
    doc["unique_id"] = (sprintf(buf, "%s.heatindex", deviceId.c_str()), buf);
    doc["device_class"] = "temperature";
    doc["state_topic"] = topicHeatIndex.c_str();
    doc["availability_topic"] = topicAvailable.c_str();
    doc["force_update"] = true;
    doc["unit_of_measurement"] = "°C";
    doc["suggested_display_precision"] = 1;
    sprintf(buf, "homeassistant/sensor/%s_heatindex/config", deviceId.c_str());
    haNetwork.mqttConfig(buf, doc);

}

void DHTSensor::loop()
{

    const unsigned long ct = millis();
    if (ct >= nextReadTime)
    {
        nextReadTime = ct + readFreq;
        const float h = dht.readHumidity();
        const float t = dht.readTemperature();
        Serial.print(F("H: "));
        Serial.print(h);
        Serial.print(F(" % / "));
        Serial.print(F("T: "));
        Serial.print(t);
        Serial.print(F(" *C / "));
        if (isnan(h) || isnan(t))
        {
            Serial.println(F("Failed to read from DHT sensor!"));
            haNetwork.mqttPublish(topicAvailable.c_str(), "offline", true);
        }
        else
        {
            const float hic = dht.computeHeatIndex(t, h, false);
            Serial.print(F("HI: "));
            Serial.print(hic);
            Serial.println(F(" *C "));
            haNetwork.mqttPublish(topicAvailable.c_str(), "online", true);
            haNetwork.mqttPublish(topicTemperature.c_str(), String(t).c_str(), true);
            haNetwork.mqttPublish(topicHumidity.c_str(), String(h).c_str(), true);
            haNetwork.mqttPublish(topicHeatIndex.c_str(), String(hic).c_str(), true);
        }
        haNetwork.mqttPublish(topicUptime.c_str(), String(ct).c_str(), true);
    }

}
