
void mqttConnect() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    if (client.connect(MQTT_CLIENTID, MQTT_USER, MQTT_PASS)) {
      Serial.println(F("connected"));
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      delay(5000);
    }
  }
}
