#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

class HANetwork
{

public:
    HANetwork(const char *wifiSsid, const char *wifiPass,
        const char *mqttServer, uint16_t mqttPort, const char *mqttClientId, const char *mqttUser, const char *mqttPass,
        const char *otaHostName, const char *otaPassHash, std::function<void(const char* topic, byte* payload, unsigned int length)> mqttCallback);
    void setup();
    void loop();
    bool mqttPublish(const char *topic, const char *payload, bool retained);
    bool mqttPublish(const char *topic, JsonDocument &doc, bool retained);
    void mqttSubscribe(const char *topic);

private:
    String wifiSsid;
    String wifiPass;
    String mqttServer;
    uint16_t mqttPort;
    String mqttClientId;
    String mqttUser;
    String mqttPass;
    String otaHostName;
    String otaPassHash;
    std::function<void(const char* topic, byte* payload, unsigned int length)> mqttCallback;

    WiFiClient espClient;
    PubSubClient client;

    void wifiConnect();
    bool mqttConnect();
    void otaSetup();

};
