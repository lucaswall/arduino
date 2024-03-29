
#include <ELECHOUSE_CC1101_SRC_DRV.h>

#include "HASomfyRemote.h"

HASomfyRemote::HASomfyRemote(int remoteNumber, PubSubClient* mqttClientObj, byte emitterPin, uint32_t remoteAddress, int rollingCodeStorageAddress) {
    Serial.print("New Somfy remote number = ");
    Serial.print(remoteNumber, DEC);
    Serial.print(", emitter pin = ");
    Serial.print(emitterPin, DEC);
    Serial.print(", remote = ");
    Serial.print(remoteAddress, HEX);
    Serial.print(", eeprom address = ");
    Serial.println(rollingCodeStorageAddress, DEC);
    remoteNum = remoteNumber;
    mqttClient = mqttClientObj;
    codeStorage = new EEPROMRollingCodeStorage(rollingCodeStorageAddress);
    remote = new SomfyRemote(emitterPin, remoteAddress, codeStorage);
    sprintf(topicCommand, "wemos_somfy_remote/remote%d/button", remoteNum);
    sprintf(topicState, "wemos_somfy_remote/remote%d/state", remoteNum);
    sprintf(topicMyState, "wemos_somfy_remote/remote%d/my_state", remoteNum);
}

HASomfyRemote::~HASomfyRemote() {
    if (remote) delete remote;
    if (codeStorage) delete codeStorage;
}

void HASomfyRemote::registerDevice() {

    Serial.print("======= *** Registering remote ");
    Serial.println(remoteNum, DEC);

    remote->setup();

    StaticJsonDocument<512> doc;

    // device information
    JsonObject device = doc.createNestedObject("device");
    char deviceIdentifier[50];
    sprintf(deviceIdentifier, "wemos_somfy_remote%d", remoteNum);
    device["identifiers"][0] = deviceIdentifier;
    char deviceName[50];
    sprintf(deviceName, "Somfy Remote%d", remoteNum);
    device["name"] = deviceName;
    device["model"] = "Wemos Somfy Remote";
    device["manufacturer"] = "MLCM Tech";

    // Prog button
    char buttonProgUniqueId[50];
    sprintf(buttonProgUniqueId, "wemos_somfy_remote%d_prog", remoteNum);
    doc["unique_id"] = buttonProgUniqueId;
    char buttonProgName[50];
    sprintf(buttonProgName, "Prog", remoteNum);
    doc["name"] = buttonProgName;
    doc["command_topic"] = topicCommand;
    doc["payload_press"] = "Prog";
    char buttonProgConfig[100];
    sprintf(buttonProgConfig, "homeassistant/button/wemos_somfy_remote%d_prog/config", remoteNum);
    sendMqttConfig(buttonProgConfig, doc);
    doc.remove("payload_press");

    // My button - Google Home will not handle button correctly, so using a switch
    char buttonMyUniqueId[50];
    sprintf(buttonMyUniqueId, "wemos_somfy_remote%d_my", remoteNum);
    doc["unique_id"] = buttonMyUniqueId;
    char buttonMyName[50];
    sprintf(buttonMyName, "My", remoteNum);
    doc["name"] = buttonMyName;
    doc["command_topic"] = topicCommand;
    doc["state_topic"] = topicMyState;
    doc["payload_on"] = "My";
    doc["payload_off"] = "My";
    doc["assumed_state"] = false;
    char buttonMyConfig[100];
    sprintf(buttonMyConfig, "homeassistant/switch/wemos_somfy_remote%d_my/config", remoteNum);
    sendMqttConfig(buttonMyConfig, doc);
    doc.remove("payload_on");
    doc.remove("payload_off");

    // Full remote (Up, Down and My) - Google Home will not expose the My button in a convenient way
    char remoteUniqueId[50];
    sprintf(remoteUniqueId, "wemos_somfy_remote%d_cover", remoteNum);
    doc["unique_id"] = remoteUniqueId;
    doc["name"] = NULL;
    doc["command_topic"] = topicCommand;
    doc["state_topic"] = topicState;
    doc["payload_open"] = "Up";
    doc["payload_close"] = "Down";
    doc["payload_stop"] = "My";
    doc["assumed_state"] = true;
    char remoteConfig[100];
    sprintf(remoteConfig, "homeassistant/cover/wemos_somfy_remote%d_cover/config", remoteNum);
    sendMqttConfig(remoteConfig, doc);
}

void HASomfyRemote::sendMqttConfig(PubSubClient& mqttClient, const char* topic, StaticJsonDocument<512>& doc) {
    String output;
    serializeJson(doc, output);
    Serial.println(output);
    Serial.print("sending config ... ");
    boolean r = mqttClient.publish(topic, output.c_str(), true);
    Serial.println(r);
}

void HASomfyRemote::sendMqttConfig(const char* topic, StaticJsonDocument<512>& doc) {
    sendMqttConfig(*mqttClient, topic, doc);
}

const char* HASomfyRemote::commandToString(Command command) {
    switch (command) {
        default: return "UNKOWN REMOTE COMMAND";
	    case Command::My: return "My";
        case Command::Up: return "Up";
        case Command::MyUp: return "MyUp";
        case Command::Down: return "Down";
        case Command::MyDown: return "MyDown";
        case Command::UpDown: return "UpDown";
        case Command::Prog: return "Prog";
        case Command::SunFlag: return "SunFlag";
        case Command::Flag: return "Flag";
    }
    return "UNKOWN REMOTE COMMAND";
}

void HASomfyRemote::sendCC1101Command(Command command) {
    Serial.print("Sending command ");
    Serial.print(commandToString(command));
    Serial.print(" to remote ");
    Serial.println(remoteNum, DEC);
    ELECHOUSE_cc1101.SetTx();
    remote->sendCommand(command);
    ELECHOUSE_cc1101.setSidle();
}

void HASomfyRemote::mqttCallback(const char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, topicCommand) == 0) {
        if (strncasecmp((char*)payload, "up", length) == 0) {
            sendCC1101Command(Command::Up);
            delay(500);
            mqttClient->publish(topicState, "open");
        }
        else if (strncasecmp((char*)payload, "my", length) == 0) {
            sendCC1101Command(Command::My);
            delay(500);
            mqttClient->publish(topicMyState, "off");
        }
        else if (strncasecmp((char*)payload, "down", length) == 0) {
            sendCC1101Command(Command::Down);
            delay(500);
            mqttClient->publish(topicState, "closed");
        }
        else if (strncasecmp((char*)payload, "prog", length) == 0) sendCC1101Command(Command::Prog);
        else Serial.println("ERROR! unkown button!");
    }
}

void HASomfyRemote::mqttSubscribe() {
    Serial.print("Subscribing to ");
    Serial.println(topicCommand);
    mqttClient->subscribe(topicCommand);
}
