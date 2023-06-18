
#include "HANetwork.h"

HANetwork::HANetwork(const char *wifiSsidInit, const char *wifiPassInit,
                     const char *mqttServerInit, uint16_t mqttPortInit, const char *mqttClientIdInit, const char *mqttUserInit, const char *mqttPassInit,
                     const char *otaHostNameInit, const char *otaPassHashInit)
        : client(espClient)
{
    wifiSsid = wifiSsidInit;
    wifiPass = wifiPassInit;
    mqttServer = mqttServerInit;
    mqttPort = mqttPortInit;
    mqttClientId = mqttClientIdInit;
    mqttUser = mqttUserInit;
    mqttPass = mqttPassInit;
    otaHostName = otaHostNameInit;
    otaPassHash = otaPassHashInit;
}

void HANetwork::setup()
{
    if (!client.setBufferSize(1024)) {
        Serial.println("ERROR! failed to alloc mqtt buffer");
    }
    wifiConnect();
    mqttConnect();
    otaSetup();
}

void HANetwork::loop()
{
    while (!client.connected())
    {
        if (WiFi.status() != WL_CONNECTED)
            wifiConnect();
        if (!mqttConnect())
            delay(1000);
    }
    client.loop();
    ArduinoOTA.handle();
}

bool HANetwork::mqttPublish(const char *topic, const char *payload, bool retained)
{
    return client.publish(topic, payload, retained);
}

void HANetwork::mqttConfig(const char *topic, StaticJsonDocument<512> &doc)
{
    String output;
    serializeJson(doc, output);
    Serial.println(topic);
    Serial.println(output);
    Serial.print("sending config ... ");
    boolean r = client.publish(topic, output.c_str(), true);
    Serial.println(r);
}

void HANetwork::wifiConnect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSsid, wifiPass);
    Serial.print(F("Connecting to "));
    Serial.print(wifiSsid);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(F("."));
    }
    Serial.println();
    Serial.print(F("Connected, IP address: "));
    Serial.println(WiFi.localIP());
}

bool HANetwork::mqttConnect()
{
    client.setServer(mqttServer.c_str(), mqttPort);
    if (client.connected())
        return true;
    Serial.print(F("Attempting MQTT connection..."));
    if (client.connect(mqttClientId.c_str(), mqttUser.c_str(), mqttPass.c_str()))
    {
        Serial.println(F("connected"));
        return true;
    }
    Serial.print(F("failed, rc="));
    Serial.println(client.state());
    return false;
}

void HANetwork::otaSetup()
{
    ArduinoOTA.setHostname(otaHostName.c_str());
    ArduinoOTA.setPasswordHash(otaPassHash.c_str());
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {  // U_FS
            type = "filesystem";
        }
        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
        delay(5000);
        ESP.restart();
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin();
}
