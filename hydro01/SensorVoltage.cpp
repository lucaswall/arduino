
#include "Arduino.h"
#include "SensorVoltage.h"
#include "config.h"

SensorVoltage::SensorVoltage(int pin, int readCount, int readDelay)
{
    this->pin = pin;
    this->readCount = readCount;
    this->readDelay = readDelay;
}

void SensorVoltage::init(SensorManager* sensorManager)
{
    this->sensorManager = sensorManager;
    pinMode(pin, INPUT);
}

void SensorVoltage::loop()
{
    if (pendingReadCount > 0 && millis() >= nextReadTime)
    {
        pendingReadCount--;
        if (pendingReadCount > 0)
        {
            nextReadTime = millis() + readDelay;
            pendingReading += analogRead(pin);
        }
        else
        {
            Serial.print(F("Finished reading pin ")); Serial.print(pin);
            pendingReading /= readCount;
            Serial.print(F(", read = ")); Serial.print(pendingReading);
            const float voltage = pendingReading * (VREF / MAXADC);
            Serial.print(F(", V = ")); Serial.print(voltage);
            lastReading = calculateValue(voltage);
            Serial.print(F(", value = ")); Serial.println(lastReading);
        }
    }
}

void SensorVoltage::startReading()
{
    Serial.print(F("Start reading pin ")); Serial.println(pin);
    pendingReadCount = readCount;
    nextReadTime = millis() + readDelay;
    pendingReading = analogRead(pin);;
}
