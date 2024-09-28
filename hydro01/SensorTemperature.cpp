
#include "SensorTemperature.h";

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

void SensorTemperature::init()
{
    sensor->begin();
}

float SensorTemperature::read(SensorManager* sensorManager)
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
    return lastTemperature;
}
