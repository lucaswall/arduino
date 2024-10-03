
#include <Arduino.h>
#include "SwitchSensor.h"

SwitchSensor::SwitchSensor(int pin)
{
    this->pin = pin;
}

void SwitchSensor::init()
{
    pinMode(pin, OUTPUT);
    off();
}

void SwitchSensor::on()
{
    Serial.print(F("SwitchSensor::on pin "));
    Serial.println(pin);
    digitalWrite(pin, LOW);
}

void SwitchSensor::off()
{
    Serial.print(F("SwitchSensor::off pin "));
    Serial.println(pin);
    digitalWrite(pin, HIGH);
}
