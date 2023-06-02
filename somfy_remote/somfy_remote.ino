
#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SomfyRemote.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "secrets.h"

#define EMITTER_GPIO D1
#define EEPROM_ADDRESS 0
#define REMOTE REMOTE_BASE
#define CC1101_FREQUENCY 433.42
#define LOOP_DELAY 100 // general loop final delay in ms
const char *MQTT_CLIENTID = "wemos_somfy_remote";
const char *OTA_HOSTNAME = "wemos_somfy_remote";

EEPROMRollingCodeStorage rollingCodeStorage(EEPROM_ADDRESS);
SomfyRemote somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage);
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println("begin");

    if (ELECHOUSE_cc1101.getCC1101())
    { // Check the CC1101 Spi connection.
        Serial.println("CC1101 SPI Connection OK");
    }
    else
    {
        Serial.println("CC1101 SPI Connection Error");
    }

    ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
    ELECHOUSE_cc1101.setGDO0(EMITTER_GPIO); // set lib internal gdo pin (gdo0). Gdo2 not use for this example.
    // ELECHOUSE_cc1101.setCCMode(1);            // set config for internal transmission mode.
    // ELECHOUSE_cc1101.setModulation(2);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    ELECHOUSE_cc1101.setMHZ(CC1101_FREQUENCY); // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    // ELECHOUSE_cc1101.setSyncMode(2);  // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
    // ELECHOUSE_cc1101.setPA(10);      // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
    // ELECHOUSE_cc1101.setCrc(1);     // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
    Serial.println("Tx Mode");

    somfyRemote.setup();
    EEPROM.begin(4);

    if (!client.setBufferSize(1024)) {
        Serial.println("ERROR! failed to alloc mqtt buffer");
    }
    if (WiFi.status() != WL_CONNECTED)
        wifiConnect();
    while (!client.connected()) {
        if (!mqttConnect())
            delay(1000);
    }
    otaSetup();

    StaticJsonDocument<512> doc;

    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = "wemos_somfy_remote0";
    device["name"] = "Somfy Remote0";
    device["model"] = "Wemos Somfy Remote";
    device["manufacturer"] = "MLCM Tech";

    doc["unique_id"] = "wemos_somfy_remote0_my";
    doc["name"] = "Somfy Remote0 My";
    doc["command_topic"] = "wemos_somfy_remote/remote0/button";
    doc["payload_on"] = "My";
    doc["payload_off"] = "My";
    sendMqttConfig("homeassistant/switch/wemos_somfy_remote0_my/config", doc);

    doc["unique_id"] = "wemos_somfy_remote0_prog";
    doc["name"] = "Somfy Remote0 Prog";
    doc["command_topic"] = "wemos_somfy_remote/remote0/button";
    doc["payload_press"] = "Prog";
    sendMqttConfig("homeassistant/button/wemos_somfy_remote0_prog/config", doc);

    doc["unique_id"] = "wemos_somfy_remote0_cover";
    doc["name"] = "Somfy Remote0";
    doc["command_topic"] = "wemos_somfy_remote/remote0/button";
    doc["state_topic"] = "wemos_somfy_remote/remote0/state";
    doc["payload_open"] = "Up";
    doc["payload_close"] = "Down";
    doc["payload_stop"] = "My";
    sendMqttConfig("homeassistant/cover/wemos_somfy_remote0_cover/config", doc);

}

void sendMqttConfig(const char* topic, StaticJsonDocument<512>& doc) {
    String output;
    serializeJson(doc, output);
    Serial.println(output);
    Serial.print("sending config ... ");
    boolean r = client.publish(topic, output.c_str(), true);
    Serial.println(r);
}

void sendCC1101Command(Command command)
{
    ELECHOUSE_cc1101.SetTx();
    somfyRemote.sendCommand(command);
    ELECHOUSE_cc1101.setSidle();
}

void loop()
{

    if (WiFi.status() != WL_CONNECTED)
        wifiConnect();
    if (!client.connected())
        mqttConnect();
    
    client.loop();

    if (Serial.available() > 0)
    {
        const String string = Serial.readStringUntil('\n');
        const Command command = getSomfyCommand(string);
        sendCC1101Command(command);
        Serial.println("finished sending");
    }

    ArduinoOTA.handle();

    delay(LOOP_DELAY);
}

void mqttCallback(const char* topic, byte* payload, unsigned int length) {


    Serial.print("TOPIC: ");
    Serial.println(topic);
    Serial.print("PAYLOAD: ");
    Serial.write((char*)payload, length);
    Serial.println();

    if (strcmp(topic, "wemos_somfy_remote/remote0/button") == 0) {
        if (strncasecmp((char*)payload, "up", length) == 0) {
            sendCC1101Command(Command::Up);
            client.publish("wemos_somfy_remote/remote0/state", "open");
        }
        else if (strncasecmp((char*)payload, "my", length) == 0) sendCC1101Command(Command::My);
        else if (strncasecmp((char*)payload, "down", length) == 0) {
            sendCC1101Command(Command::Down);
            client.publish("wemos_somfy_remote/remote0/state", "closed");
        }
        else if (strncasecmp((char*)payload, "prog", length) == 0) sendCC1101Command(Command::Prog);
        else Serial.println("ERROR! unkown button!");
    }

}

void wifiConnect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print(F("Connecting to "));
    Serial.print(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println();
    Serial.print(F("Connected, IP address: "));
    Serial.println(WiFi.localIP());
}

bool mqttConnect()
{
    client.setServer(MQTT_SERVER, MQTT_PORT);
    if (client.connected())
        return true;
    Serial.print(F("Attempting MQTT connection..."));
    if (client.connect(MQTT_CLIENTID, MQTT_USER, MQTT_PASS))
    {
        Serial.println(F("connected"));
        client.setCallback(mqttCallback);
        client.subscribe("wemos_somfy_remote/remote0/button");
        return true;
    }
    Serial.print(F("failed, rc="));
    Serial.println(client.state());
    return false;
}

void otaSetup()
{
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPasswordHash(OTA_PASSHASH);
    ArduinoOTA.onStart([]()
                       {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type); });
    ArduinoOTA.onEnd([]()
                     {
    Serial.println("\nEnd");
    delay(5000);
    ESP.restart(); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
    ArduinoOTA.onError([](ota_error_t error)
                       {
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
    } });
    ArduinoOTA.begin();
}
