
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_TSL2591.h>

#include "secrets.h"

const char* MQTT_CLIENTID = "wemos_garden02";
const unsigned long sleepTime = 60 * 5;
const int batterySensor = A0;

bool tsl2591Init = false;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(1);

void setupSensor() {
  if (tsl.begin()) {
    Serial.println(F("TSL2591 sensor found!"));
  } else {
    Serial.println(F("ERROR!! TSL2591 sensor NOT FOUND!"));
    tsl2591Init = false;
    return;
  }
  tsl.setGain(TSL2591_GAIN_LOW);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  tsl2591Init = true;
}

void readSensor() {
  const unsigned long startTime = millis();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  if (WiFi.status() != WL_CONNECTED) wifiConnect();
  if (!client.connected()) mqttConnect();
  if (!tsl2591Init) setupSensor();

  if (tsl2591Init) {
    tsl.getFullLuminosity();
    const uint32_t lum = tsl.getFullLuminosity();
    const uint16_t ir = lum >> 16;
    const uint16_t full = lum & 0xFFFF;
    const uint16_t visible = full - ir;
    const float lux = tsl.calculateLux(full, ir);
    Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
    Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
    Serial.print(F("Visible: ")); Serial.print(visible); Serial.print(F("  "));
    Serial.print(F("Lux: ")); Serial.println(lux, 0);
    client.publish("garden02/tsl2591/full", String(full).c_str(), true);
    client.publish("garden02/tsl2591/visible", String(visible).c_str(), true);
    client.publish("garden02/tsl2591/ir", String(ir).c_str(), true);
    client.publish("garden02/tsl2591/illuminance", String(lux, 0).c_str(), true);
    client.publish("garden02/tsl2591/available", "online", true);
  } else {
    Serial.println(F("TSL2591 is not availiable for reading..."));
    client.publish("garden02/tsl2591/available", "offline", true);
  }
  
  float batteryValue = (analogRead(batterySensor) * 5.0f) / 1023.0f;
  Serial.print(F("batteryValue = ")); Serial.print(batteryValue, 2); Serial.println(F(""));
  client.publish("garden02/battery", String(batteryValue, 2).c_str(), true);

  const unsigned long loopTime = millis() - startTime;
  Serial.print(F("loopTime = ")); Serial.print(loopTime); Serial.println(F("ms"));
  client.publish("garden02/looptime", String(loopTime).c_str(), true);
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("!"));
  Serial.println(F("Booting!"));

  readSensor();

  delay(500);
  Serial.println(F("Sleep!"));
  //delay(sleepTime * 1000); ESP.restart();
  ESP.deepSleep(sleepTime * 1000000);
}

void loop() {
}
