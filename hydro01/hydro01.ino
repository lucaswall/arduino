
#include "HANetwork.h"
#include "SensorManager.h"
#include "MqttDevice.h"

const char *MQTT_CLIENTID = "hydro01";
const char *OTA_HOSTNAME = "hydro01";

#include "secrets.h"
void mqttCallback(const char* topic, byte* payload, unsigned int length);
HANetwork haNetwork(WIFI_SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, MQTT_CLIENTID, MQTT_USER, MQTT_PASS, OTA_HOSTNAME, OTA_PASSHASH, mqttCallback);

SensorManager sensorManager;
MqttDevice mqttDevice(&haNetwork);

void setup()
{
    Serial.begin(115200);
    Serial.println(F("."));
    delay(2000);
    Serial.println(F("======================= BEGIN ======================="));
    sensorManager.init(&mqttDevice);
    haNetwork.setup();
    mqttDevice.registerDevice();
}

void loop()
{
    haNetwork.loop();
    sensorManager.loop();
    delay(100);
}

void mqttCallback(const char* topic, byte* payload, unsigned int length) {

    Serial.print(F("TOPIC: "));
    Serial.println(topic);
    Serial.print(F("PAYLOAD: "));
    Serial.write((char*)payload, length);
    Serial.println();

}
