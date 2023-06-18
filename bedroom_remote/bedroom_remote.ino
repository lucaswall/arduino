
// #define DEBUG
#define DECODE_NEC

#include <IRremote.hpp>

#include "secrets.h"
#include "HANetwork.h"

#define IR_RECEIVE_PIN D5
#define LOOP_DELAY 100 // general loop final delay in ms
const char *MQTT_CLIENTID = "bedroom_remote";
const char *OTA_HOSTNAME = "bedroom_remote";
const char *deviceId = "bedroom_remote";
const char *roomName = "Bedroom";
const char *remoteButtonTopic = "bedroom_remote/remote_button";

HANetwork haNetwork(WIFI_SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, MQTT_CLIENTID, MQTT_USER, MQTT_PASS, OTA_HOSTNAME, OTA_PASSHASH);

void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("Booting"));
    IrReceiver.begin(IR_RECEIVE_PIN);
    Serial.print("Active protocolos ");
    printActiveIRProtocols(&Serial);
    Serial.print("Data Pin = ");
    Serial.println(IR_RECEIVE_PIN);
    haNetwork.setup();
    registerMqttDevice();
}

void loop()
{
    haNetwork.loop();
    if (IrReceiver.decode())
    {
        IrReceiver.printIRResultShort(&Serial);
        processIRCommand(IrReceiver.decodedIRData);
        IrReceiver.resume();
    }
    delay(LOOP_DELAY);
}

#define REMOTE_NUM1 0x45
#define REMOTE_NUM2 0x46
#define REMOTE_NUM3 0x47
#define REMOTE_NUM4 0x44
#define REMOTE_NUM5 0x40
#define REMOTE_NUM6 0x43
#define REMOTE_NUM7 0x07
#define REMOTE_NUM8 0x15
#define REMOTE_NUM9 0x09
#define REMOTE_NUM0 0x19
#define REMOTE_STAR 0x16
#define REMOTE_POUND 0x0D
#define REMOTE_UP 0x18
#define REMOTE_DOWN 0x52
#define REMOTE_LEFT 0x08
#define REMOTE_RIGHT 0x5A
#define REMOTE_OK 0x1C

typedef struct {
    uint16_t command;
    const char *payload;
} CommandPayloadType;

const CommandPayloadType cmdsPayloads[] = {
    { REMOTE_NUM1, "num1" },
    { REMOTE_NUM2, "num2" },
    { REMOTE_NUM3, "num3" },
    { REMOTE_NUM4, "num4" },
    { REMOTE_NUM5, "num5" },
    { REMOTE_NUM6, "num6" },
    { REMOTE_NUM7, "num7" },
    { REMOTE_NUM8, "num8" },
    { REMOTE_NUM9, "num9" },
    { REMOTE_NUM0, "num0" },
    { REMOTE_STAR, "star" },
    { REMOTE_POUND, "pound" },
    { REMOTE_UP, "up" },
    { REMOTE_DOWN, "down" },
    { REMOTE_LEFT, "left" },
    { REMOTE_RIGHT, "right" },
    { REMOTE_OK, "ok" },
    { 0, nullptr },
};

void registerMqttDevice()
{
    StaticJsonDocument<512> doc;
    char buf[500];

    // device information
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = deviceId;
    device["name"] = "Bedroom Remote";
    device["model"] = "Wemos IR Remote";
    device["manufacturer"] = "MLCM Tech";
    device["suggested_area"] = roomName;

    for (int cmdIdx = 0; cmdsPayloads[cmdIdx].command != 0; cmdIdx++)
    {
        doc["automation_type"] = "trigger";
        doc["topic"] = remoteButtonTopic;
        doc["type"] = "button_press";
        doc["subtype"] = cmdsPayloads[cmdIdx].payload;
        doc["payload"] = cmdsPayloads[cmdIdx].payload;
        sprintf(buf, "homeassistant/device_automation/bedroom_remote_%s/config", cmdsPayloads[cmdIdx].payload);
        haNetwork.mqttConfig(buf, doc);
    }
}

void processIRCommand(const IRData &data)
{
    if (data.flags & IRDATA_FLAGS_IS_REPEAT) return;
    int cmdIdx = 0;
    for ( ; cmdsPayloads[cmdIdx].command != 0; cmdIdx++)
    {
        if (data.command == cmdsPayloads[cmdIdx].command)
            break;
    }
    if (cmdsPayloads[cmdIdx].command == 0)
    {
        Serial.println("Unkown command");
        return;
    }
    Serial.print("Command payload = ");
    Serial.println(cmdsPayloads[cmdIdx].payload);
    haNetwork.mqttPublish(remoteButtonTopic, cmdsPayloads[cmdIdx].payload, true);
}
