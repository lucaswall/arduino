
#include "secrets.h"
#include "HANetwork.h"
#include "XmasLights.h"

#define R1 D7
#define R2 D2
#define R3 D3
#define R4 D4

const char *MQTT_CLIENTID = "xmas_light";
const char *OTA_HOSTNAME = "xmas_light";
void mqttCallback(const char* topic, byte* payload, unsigned int length);
const char *topicCommand = "xmas_light/set";
const char *topicState = "xmas_light/state";

HANetwork haNetwork(WIFI_SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, MQTT_CLIENTID, MQTT_USER, MQTT_PASS, OTA_HOSTNAME, OTA_PASSHASH, mqttCallback);

int pattern1[] = { 3000, 1000, -1 };

#define LIGHTS_EFFECT_NAME_MAX 100

XmasLights lights1(D7);
unsigned long lastLoop;
bool lightsOn;
char lightsEffect[LIGHTS_EFFECT_NAME_MAX];

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("Booting"));

    lights1.init();
    //lights1.setPattern(pattern1);
    lastLoop = millis();
    lightsOn = false;
    strncpy(lightsEffect, "Always On", LIGHTS_EFFECT_NAME_MAX);

    haNetwork.setup();
    registerMqttDevice();
    sendCurrentState();
}

void loop() {

    haNetwork.loop();

    unsigned long ct = millis();
    unsigned long dt = ct > lastLoop ? ct - lastLoop : 0;
    lastLoop = millis();

    lights1.loop(dt);

    delay(100);

}

void registerMqttDevice()
{
    StaticJsonDocument<512> doc;

    Serial.println(F("Registering Xmas Light MQTT device"));

    // device information
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = "xmas_light";
    device["name"] = "Xmas Light Controller";
    device["model"] = "Wemos Xmas Light";
    device["manufacturer"] = "MLCM Tech";
    device["suggested_area"] = "Living Room";

    doc["schema"] = "json";
    doc["name"] = "Xmas Light";
    doc["object_id"] = "xmas_light";
    doc["unique_id"] = "xmas_light";
    doc["command_topic"] = topicCommand;
    doc["state_topic"] = topicState;

    StaticJsonDocument<512> effectList;
    doc["effect"] = true;
    effectList.add("Always On");
    effectList.add("Pattern 1");
    doc["effect_list"] = effectList;

    haNetwork.mqttPublish("homeassistant/light/xmas_light/config", doc, true);
    haNetwork.mqttSubscribe(topicCommand);

}

void setLightsPattern(const char *effect) {
    if (strncasecmp(effect, "Always On", LIGHTS_EFFECT_NAME_MAX) == 0) {
        lights1.clearPattern();
        lights1.on();
    } else if (strncasecmp(effect, "Pattern 1", LIGHTS_EFFECT_NAME_MAX) == 0) {
        lights1.setPattern(pattern1);
    }
}

void mqttCallback(const char* topic, byte* payload, unsigned int length) {

    Serial.print(F("TOPIC: "));
    Serial.println(topic);
    Serial.print(F("PAYLOAD: "));
    Serial.write((char*)payload, length);
    Serial.println();

    if (strcmp(topic, topicCommand) == 0) {
        DynamicJsonDocument doc(1024);
        DeserializationError err = deserializeJson(doc, payload, length);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.f_str());
        } else {
            const char *state = doc["state"];
            if (state) {
                lightsOn = strcasecmp(state, "ON") == 0;
                if (lightsOn) {
                    setLightsPattern(lightsEffect);
                } else {
                    lights1.clearPattern();
                }
            }
            const char *effect = doc["effect"];
            if (effect) {
                strncpy(lightsEffect, effect, LIGHTS_EFFECT_NAME_MAX);
                if (lightsOn) setLightsPattern(lightsEffect);
            }
        }
    }

    sendCurrentState();

}

void sendCurrentState() {
    StaticJsonDocument<512> doc;
    doc["state"] = lightsOn ? "ON" : "OFF";
    doc["effect"] = lightsEffect;
    haNetwork.mqttPublish(topicState, doc, true);
}
