
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// https://github.com/esp8266/Arduino/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino
// https://www.hackster.io/feiticeir0/luminosity-sensor-for-home-assistant-119da1
// https://pubsubclient.knolleary.net/

#include "secrets.h"

const char* OTA_HOSTNAME = "wemos_test01";
const char* MQTT_CLIENTID = "wemos_test01";

const int lightSensor = A0;

bool blinkLed = true;

WiFiClient espClient;
PubSubClient client(espClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if ((char)payload[0] == '1') {
    blinkLed = true;
  } else {
    blinkLed = false;
  }
  client.publish("test01/blink", blinkLed ? "1" : "0", true);
}

void mqttSetup() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);
}

void mqttConnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENTID, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      unsigned long upTime = millis();
      client.publish("test01/uptime", String(upTime).c_str(), true);
      client.publish("test01/blink", blinkLed ? "1" : "0", true);
      client.subscribe("test01/blink/set");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void otaSetup() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPasswordHash(OTA_PASSHASH);
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
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
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

void setup() {

  delay(1000);
  Serial.begin(115200);
  Serial.println("!");
  delay(1000);
  Serial.println("Booting");

  randomSeed(micros());
  wifiConnect();
  otaSetup();
  mqttSetup();

  pinMode(LED_BUILTIN, OUTPUT);

}

const float VCC = 3.3;
const int R = 10 * 1000;
const int ADCmax = 1023;

void loop() {
 
  ArduinoOTA.handle();

  if (!client.connected()) {
    digitalWrite(LED_BUILTIN, LOW);
    mqttConnect();
    digitalWrite(LED_BUILTIN, HIGH);
  }
  client.loop();

  unsigned long upTime = millis();
  client.publish("test01/uptime", String(upTime).c_str(), true);
  client.publish("test01/blink", blinkLed ? "1" : "0", true);

  int lightValue = analogRead(lightSensor);
  float voltageDrop = (float)VCC / 1023 * lightValue;
  float LDR = R / voltageDrop * (((float)VCC - voltageDrop) / 1000);
  Serial.print("lightValue = ");
  Serial.print(lightValue);
  Serial.print(", voltageDrop = ");
  Serial.print(voltageDrop);
  Serial.print("V, LDR = ");
  Serial.print(LDR);
  Serial.println("k");

  if (blinkLed) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  }

}
