#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

class HANetwork
{

public:
    HANetwork(const char *wifiSsid, const char *wifiPass,
        const char *mqttServer, uint16_t mqttPort, const char *mqttClientId, const char *mqttUser, const char *mqttPass,
        const char *otaHostName, const char *otaPassHash);
    void setup();
    void loop();
    bool mqttPublish(const char *topic, const char *payload, bool retained);
    void mqttConfig(const char *topic, StaticJsonDocument<512> &doc);

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

    WiFiClient espClient;
    PubSubClient client;

    void wifiConnect();
    bool mqttConnect();
    void otaSetup();

};
