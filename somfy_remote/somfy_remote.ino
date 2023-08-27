
// Copyright (C) 2023 Lucas Wall <wall.lucas@gmail.com>

#include <EEPROM.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoOTA.h>

#include "secrets.h"
#include "HASomfyRemote.h"

// E07-M1101D -> Wemos mini (PINS)
// E07-M1101D-SMA_Usermanual_EN_v1.30.pdf
// https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/img/Esp8266_CC1101.png
// 1 (GND)       -> G
// 2 (VCC)       -> 3V3
// 3 (GDO0)      -> D1
// 4 (CSN)       -> D8
// 5 (SCK)       -> D5
// 6 (MOSI)      -> D7
// 7 (MISO/GDO1) -> D6
// 8 (GDO2)      -> D2

#define EMITTER_GPIO D1
#define EEPROM_SIZE 64
#define EEPROM_REMOTE_ADDRESS 0
#define EEPROM_COUNT_ADDRESS 60
#define MAX_REMOTES_COUNT 30
#define INIT_REMOTES_COUNT 5
#define REMOTE REMOTE_BASE
#define CC1101_FREQUENCY 433.42
#define LOOP_DELAY 100 // general loop final delay in ms
const char *MQTT_CLIENTID = "wemos_somfy_remote";
const char *OTA_HOSTNAME = "wemos_somfy_remote";
const char *MQTT_TOPIC_REMOTES_COMMAND = "wemos_somfy_remote/remotes/command";

WiFiClient espClient;
PubSubClient client(espClient);
uint16_t remoteCount;
HASomfyRemote *remotes[MAX_REMOTES_COUNT];

#define HAVE_ADD_REMOTE_BUTTON 0

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

    EEPROM.begin(EEPROM_SIZE);
    client.setCallback(mqttCallback);

    EEPROM.get(EEPROM_COUNT_ADDRESS, remoteCount);
    Serial.print("Remotes count = ");
    Serial.println(remoteCount);
    if (remoteCount > MAX_REMOTES_COUNT || remoteCount < INIT_REMOTES_COUNT) {
        Serial.println("Initializing remote count");
        remoteCount = INIT_REMOTES_COUNT;
        EEPROM.put(EEPROM_COUNT_ADDRESS, remoteCount);
        EEPROM.commit();
        Serial.print("Remotes count = ");
        Serial.println(remoteCount);
    }

    for (int i = 0; i < remoteCount; i++) {
        remotes[i] = new HASomfyRemote(i, &client, EMITTER_GPIO, REMOTE_BASE+i, EEPROM_REMOTE_ADDRESS+(i*2));
    }
    if (!client.setBufferSize(1024)) {
        Serial.println("ERROR! failed to alloc mqtt buffer");
    }
    while (!client.connected()) {
        if (WiFi.status() != WL_CONNECTED)
            wifiConnect();
        if (!mqttConnect())
            delay(1000);
    }
    otaSetup();

    for (int i = 0; i < remoteCount; i++) {
        remotes[i]->registerDevice();
    }

#if HAVE_ADD_REMOTE_BUTTON
    StaticJsonDocument<512> doc;

    // device information
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = "wemos_somfy_remotes";
    device["name"] = "Somfy Remotes Controller";
    device["model"] = "Wemos Somfy Remote";
    device["manufacturer"] = "MLCM Tech";

    // Add remote button
    doc["unique_id"] = "wemos_somfy_remotes_add";
    doc["name"] = "Add";
    doc["command_topic"] = MQTT_TOPIC_REMOTES_COMMAND;
    doc["payload_press"] = "Add";
    HASomfyRemote::sendMqttConfig(client, "homeassistant/button/wemos_somfy_remotes_add/config", doc);
#else
    Serial.print("clearing add remote button ... ");
    boolean r = client.publish("homeassistant/button/wemos_somfy_remotes_add/config", "", true);
    Serial.println(r);
#endif

}

void loop()
{

    while (!client.connected()) {
        if (WiFi.status() != WL_CONNECTED)
            wifiConnect();
        if (!mqttConnect())
            delay(1000);
    }
    
    client.loop();

    if (Serial.available() > 0)
    {
        const String string = Serial.readStringUntil('\n');
        const Command command = getSomfyCommand(string);
        remotes[0]->sendCommand(command);
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

    for (int i = 0; i < remoteCount; i++) remotes[i]->mqttCallback(topic, payload, length);

    if (strcmp(topic, MQTT_TOPIC_REMOTES_COMMAND) == 0) {
        if (strncasecmp((char*)payload, "add", length) == 0) {
            int remoteNum = remoteCount++;
            EEPROM.put(EEPROM_COUNT_ADDRESS, remoteNum);
            EEPROM.commit();
            remotes[remoteNum] = new HASomfyRemote(remoteNum, &client, EMITTER_GPIO, REMOTE_BASE+remoteNum, EEPROM_REMOTE_ADDRESS+(remoteNum*2));
            remotes[remoteNum]->registerDevice();
            remotes[remoteNum]->mqttSubscribe();
        }
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
        client.subscribe(MQTT_TOPIC_REMOTES_COMMAND);
        for (int i = 0; i < remoteCount; i++) remotes[i]->mqttSubscribe();
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
