
bool mqttConnect() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  if (client.connected()) return true;
  Serial.print(F("Attempting MQTT connection..."));
  if (client.connect(MQTT_CLIENTID, MQTT_USER, MQTT_PASS)) {
    Serial.println(F("connected"));
    return true;
  }
  Serial.print(F("failed, rc="));
  Serial.println(client.state());
  return false;
}
