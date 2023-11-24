
#include "secrets.h"
#include "HANetwork.h"
#include "XmasLights.h"

const char *MQTT_CLIENTID = "xmas_light";
const char *OTA_HOSTNAME = "xmas_light";
void mqttCallback(const char* topic, byte* payload, unsigned int length);
const char *topicCommand = "xmas_light/set";
const char *topicState = "xmas_light/state";

HANetwork haNetwork(WIFI_SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, MQTT_CLIENTID, MQTT_USER, MQTT_PASS, OTA_HOSTNAME, OTA_PASSHASH, mqttCallback);

#define LIGHTS_EFFECT_NAME_MAX 100

#define LIGHTS_COUNT 4
XmasLights lights[LIGHTS_COUNT] = {
    XmasLights(D7),
    XmasLights(D6),
    XmasLights(D5),
    XmasLights(D4),
};

struct PatternT {
    const char *name;
    int patterns[LIGHTS_COUNT][10];
};

PatternT patterns[] = {
    "Pattern 1", {
        { 3000, 1000, -1 },
        { 3000, 1000, -1 },
        { 3000, 1000, -1 },
        { 3000, 1000, -1 },
    },
    "Pattern 2", {
        { 3000, 1000, 1000, 3000, -1 },
        { 1000, 3000, 3000, 1000, -1 },
        { 3000, 1000, 1000, 3000, -1 },
        { 1000, 3000, 3000, 1000, -1 },
    },
    "Pattern 3", {
        { 3000, 1000, 2400, 1600, -1 },
        { 2800, 1200, 2600, 1400, -1 },
        { 2600, 1400, 2800, 1200, -1 },
        { 2400, 1600, 3000, 1000, -1 },
    },
    nullptr,
};

unsigned long lastLoop;
bool lightsOn;
char lightsEffect[LIGHTS_EFFECT_NAME_MAX];

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("\n\nBooting"));

    for (int i = 0; i < LIGHTS_COUNT; i++) lights[i].init();
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

    for (int i = 0; i < LIGHTS_COUNT; i++) lights[i].loop(dt);

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
    for (int p = 0; patterns[p].name != nullptr; p++)
        effectList.add(patterns[p].name);
    doc["effect_list"] = effectList;

    haNetwork.mqttPublish("homeassistant/light/xmas_light/config", doc, true);
    haNetwork.mqttSubscribe(topicCommand);

}

void setLightsPattern(const char *effect) {
    for (int p = 0; patterns[p].name != nullptr; p++) {
        if (strncasecmp(effect, patterns[p].name, LIGHTS_EFFECT_NAME_MAX) == 0) {
            for (int i = 0; i < LIGHTS_COUNT; i++) lights[i].setPattern(patterns[p].patterns[i]);
            return;
        }    
    }
    // Always On
    for (int i = 0; i < LIGHTS_COUNT; i++) {
        lights[i].clearPattern();
        lights[i].on();
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
                    for (int i = 0; i < LIGHTS_COUNT; i++) lights[i].clearPattern();
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
