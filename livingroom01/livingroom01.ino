
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoOTA.h>

#include "secrets.h"

#define LOOP_DELAY 100         // general loop final delay in ms
#define SENSOR_READ_FREQ (60 * 1000)  // sensor read frequency in ms
const char* MQTT_CLIENTID = "wemos_livingroom01";
const char* OTA_HOSTNAME = "wemos_livingroom01";

DHT dht(D8, DHT21);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  delay(1000);
  Serial.begin(115200);
  Serial.println(F("!"));
  delay(1000);
  Serial.println(F("Booting"));

  if (WiFi.status() != WL_CONNECTED) wifiConnect();
  otaSetup();
  
  pinMode(D8, INPUT);
  dht.begin();

}

unsigned long lastReadTime = 0;

void loop() {

  if (WiFi.status() != WL_CONNECTED) wifiConnect();

  const unsigned long ct = millis();
  if (ct < lastReadTime || ct >= lastReadTime + SENSOR_READ_FREQ) {
    lastReadTime = millis();
    if (!client.connected()) mqttConnect();
    const float h = dht.readHumidity();
    const float t = dht.readTemperature();
    Serial.print(F("H: "));
    Serial.print(h);
    Serial.print(F(" % / "));
    Serial.print(F("T: "));
    Serial.print(t);
    Serial.print(F(" *C / "));
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      client.publish("livingroom01/dht/available", "offline", true);
    } else {
      const float hic = dht.computeHeatIndex(t, h, false);
      Serial.print(F("HI: "));
      Serial.print(hic);
      Serial.println(F(" *C "));
      client.publish("livingroom01/dht/available", "online", true);
      client.publish("livingroom01/dht/temperature", String(t).c_str(), true);
      client.publish("livingroom01/dht/humidity", String(h).c_str(), true);
      client.publish("livingroom01/dht/heatindex", String(hic).c_str(), true);
    }
    client.publish("livingroom01/uptime", String(ct).c_str(), true);
  }

  ArduinoOTA.handle();

  delay(LOOP_DELAY);

}
