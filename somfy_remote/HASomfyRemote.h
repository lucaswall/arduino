#pragma once

#include <EEPROMRollingCodeStorage.h>
#include <SomfyRemote.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

class HASomfyRemote {

private:
    int remoteNum;
    PubSubClient* mqttClient;
    SomfyRemote *remote = nullptr;
    RollingCodeStorage *codeStorage = nullptr;
    char topicCommand[100];
    char topicState[100];

public:
    static void sendMqttConfig(PubSubClient& mqttClient, const char* topic, StaticJsonDocument<512>& doc);

public:
    HASomfyRemote(int remoteNumber, PubSubClient* mqttClient, byte emitterPin, uint32_t remoteAddress, int rollingCodeStorageAddress);
    virtual ~HASomfyRemote();
    void registerDevice();
    void sendCommand(Command command) { sendCC1101Command(command); }
    void mqttCallback(const char* topic, byte* payload, unsigned int length);
    void mqttSubscribe();

private:
    const char* commandToString(Command command);
    void sendCC1101Command(Command command);
    void sendMqttConfig(const char* topic, StaticJsonDocument<512>& doc);

};
