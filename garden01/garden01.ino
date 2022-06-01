
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "secrets.h"

const char* MQTT_CLIENTID = "wemos_garden01";

const unsigned long sleepTime = 5;
const int lightSensor = A0;

WiFiClient espClient;
PubSubClient client(espClient);

void readSensor() {
  const unsigned long startTime = millis();
  if (WiFi.status() != WL_CONNECTED) wifiConnect();
  if (!client.connected()) mqttConnect();
  int lightValue = analogRead(lightSensor);
  client.publish("garden01/lightsensor", String(lightValue).c_str(), true);
  const unsigned long runTime = millis() - startTime;
  client.publish("garden01/runtime", String(runTime).c_str(), true);
  Serial.print("lightValue = ");
  Serial.print(lightValue);
  Serial.print(", runTime = ");
  Serial.print(runTime);
  Serial.println("ms");
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  Serial.println("!");
  Serial.println("Booting!");
  pinMode(LED_BUILTIN, OUTPUT);
  /*
  readSensor();
  delay(500);
  ESP.deepSleep(sleepTime * 1000000);
  */
}

void loop() {
  ///*
  readSensor();
  delay(sleepTime * 1000);
  //*/
}
