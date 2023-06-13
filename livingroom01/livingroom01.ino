
#include "secrets.h"
#include "HANetwork.h"
#include "DHTSensor.h"

#define LOOP_DELAY 100               // general loop final delay in ms
#define SENSOR_READ_FREQ (60 * 1000) // sensor read frequency in ms
const char *MQTT_CLIENTID = "wemos_livingroom01";
const char *OTA_HOSTNAME = "wemos_livingroom01";
const char *deviceId = "livingroom01";
const char *roomName = "Living Room";

HANetwork haNetwork(WIFI_SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, MQTT_CLIENTID, MQTT_USER, MQTT_PASS, OTA_HOSTNAME, OTA_PASSHASH);
DHTSensor dhtSensor(haNetwork, SENSOR_READ_FREQ, D5, DHT22, deviceId, roomName);

void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("Booting"));
    haNetwork.setup();
    dhtSensor.setup();
}

void loop()
{
    haNetwork.loop();
    dhtSensor.loop();
    delay(LOOP_DELAY);
}
