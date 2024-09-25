
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>

const uint8_t sensorTempPin = 26;
const uint8_t sensorTdsPin = 39;
const uint8_t sensorPhPin = 36;
const uint8_t sensorWaterLevel = 34;

OneWire oneWireTemp(sensorTempPin);
DallasTemperature sensorTemp(&oneWireTemp);

const float VREF = 3.3;
const float MAXADC = 4095.0;
const float TdsFactor = 0.5;
const float RefTemperature = 25.0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("."));
  delay(2000);
  Serial.println(F("======================= BEGIN ======================="));
  sensorTemp.begin();
  pinMode(sensorTdsPin, INPUT);
  pinMode(sensorPhPin, INPUT);
  pinMode(sensorWaterLevel, INPUT);
}

void loop() {
  const float temp = readTemperature();
  const float ppm = readPpm(temp);
  const float ph = readPH(temp);
  const float waterLevel = readWaterLevel();
  Serial.print(F("Sensors "));
  Serial.print(temp);
  Serial.print(F(" °C, "));
  Serial.print(ppm);
  Serial.print(F(" ppm, "));
  Serial.print(ph);
  Serial.print(F(" pH, "));
  Serial.print(waterLevel);
  Serial.println(F(" %wl"));
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

float readVoltage(uint8_t pin) {
  Serial.print(F("pin read = "));
  const int readCount = 30;
  const int readDelay = 10;
  float pinRead = 0;
  for (int i = 0; i < readCount; i++) {
    pinRead += analogRead(pin);
    delay(readDelay);
  }
  pinRead /= readCount;
  Serial.print(pinRead);
  const float voltage = pinRead * (VREF / MAXADC);
  Serial.print(F(", V = ")); Serial.print(voltage);
  return voltage;
}

float readPpm(float temperature) {
  Serial.print(F("TDS "));
  const float voltage = readVoltage(sensorTdsPin);
  const float compC = 1.0 + 0.02 * (temperature - 25.0);
  const float compV = voltage / compC;
  const float tdsValue = (133.42 * compV * compV * compV - 255.86 * compV * compV + 857.39 * compV) * TdsFactor;
  Serial.print(F(", ppm = ")); Serial.println(tdsValue);
  return tdsValue;
}

float readPH(float temperature) {
  const float lowPH = 4.01; const float lowV = 2.00;
  const float midPH = 6.86; const float midV = 1.66;
  Serial.print(F("PH "));
  const float voltage = readVoltage(sensorPhPin);
  const float phValue = midPH + (voltage - midV) * ((midPH - lowPH) / (midV - lowV)) + 0.03 * (temperature - RefTemperature);
  Serial.print(F(", pH = ")); Serial.println(phValue);
  return phValue;
}

float readWaterLevel() {
  const float minLevelV = 2.72;
  const float maxLevelV = 0.55;
  Serial.print(F("Water Level "));
  const float voltage = readVoltage(sensorWaterLevel);
  const float level = voltage >= minLevelV ? 0.0 : (voltage <= maxLevelV ? 1.0 : (voltage - minLevelV) / (maxLevelV - minLevelV));
  Serial.print(F(", level = ")); Serial.println(level);
  return level;
}
