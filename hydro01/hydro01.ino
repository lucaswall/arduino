
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>

const uint8_t sensorTempPin = 26;
const uint8_t sensorTdsPin = 39;

OneWire oneWireTemp(sensorTempPin);
DallasTemperature sensorTemp(&oneWireTemp);

const float VREF = 3.3;
const float TdsFactor = 0.727;
const float RefTemperature = 25.0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("."));
  delay(2000);
  Serial.println(F("======================= BEGIN ======================="));
  sensorTemp.begin();
  pinMode(sensorTdsPin, INPUT);
}

void loop() {
  const float temp = readTemperature();
  const float ppm = readPpm(temp);
  Serial.print(F("Sensors "));
  Serial.print(temp);
  Serial.print(F(" °C, "));
  Serial.print(ppm);
  Serial.println(F(" ppm"));
  delay(1000);
}

float readTemperature() {
  Serial.print(F("Temp = "));
  sensorTemp.requestTemperatures();
  const float temp = sensorTemp.getTempCByIndex(0);
  if (temp == DEVICE_DISCONNECTED_C) {
    Serial.println(F("ERROR"));
    return RefTemperature;
  }
  Serial.println(temp);
  return temp;
}

float readPpm(float temperature) {
  Serial.print(F("TDS pin = "));
  const int readCount = 10;
  const int readDelay = 50;
  float pinRead = 0;
  for (int i = 0; i < readCount; i++) {
    pinRead += analogRead(sensorTdsPin);
  }
  pinRead /= readCount;
  Serial.print(pinRead);
  const float voltage = pinRead * (VREF / 4096.0);
  Serial.print(F(", V = ")); Serial.print(voltage);
  const float compC = 1.0 + 0.02 * (temperature - 25.0);
  const float compV = voltage / compC;
  const float tdsValue = (133.42 * compV * compV * compV - 255.86 * compV * compV + 857.39 * compV) * TdsFactor;
  Serial.print(F(", ppm = ")); Serial.println(tdsValue);
  return tdsValue;
}
