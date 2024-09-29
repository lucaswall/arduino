
#include "SensorTemperature.h"

SensorTemperature::SensorTemperature(int pin)
{
    oneWire = new OneWire(pin);
    sensor = new DallasTemperature(oneWire);
}

SensorTemperature::~SensorTemperature()
{
    delete sensor;
    delete oneWire;
}

void SensorTemperature::init(SensorManager* sensorManager)
{
    sensor->begin();
}

void SensorTemperature::startReading()
{
    Serial.print(F("Reading temperature "));
    sensor->requestTemperatures();
    lastTemperature = sensor->getTempCByIndex(0);
    if (lastTemperature == DEVICE_DISCONNECTED_C)
    {
        Serial.println(F("ERROR"));
    }
    else
    {
        Serial.println(lastTemperature);
    }
}
