
#include "Arduino.h";
#include "SensorVoltage.h";
#include "config.h";

SensorVoltage::SensorVoltage(int pin, int readCount, int readDelay)
{
    this->pin = pin;
    this->readCount = readCount;
    this->readDelay = readDelay;
}

void SensorVoltage::init()
{
    pinMode(pin, INPUT);
}

float SensorVoltage::read(SensorManager* sensorManager)
{
    Serial.print(F("reading pin ")); Serial.print(pin);
    float pinRead = 0;
    for (int i = 0; i < readCount; i++)
    {
        pinRead += analogRead(pin);
        delay(readDelay);
    }
    pinRead /= readCount;
    Serial.print(F(", read = ")); Serial.print(pinRead);
    const float voltage = pinRead * (VREF / MAXADC);
    Serial.print(F(", V = ")); Serial.print(voltage);
    const float value = calculateValue(sensorManager, voltage);
    Serial.print(F(", value = ")); Serial.println(value);
    return value;
}
