
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
    digitalWrite(pin, LOW);
}

void SwitchSensor::off()
{
    digitalWrite(pin, HIGH);
}
