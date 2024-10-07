
#include "StateReadSensor.h"
#include "SensorManager.h"

StateReadSensor::StateReadSensor(SensorManager* sensorManager, const char *name, SensorType sensorType)
    : State(sensorManager, name)
{
    this->sensorType = sensorType;
}

void StateReadSensor::enter()
{
    sensorManager->getSensor(sensorType)->startReading();
}

void StateReadSensor::loop()
{
    Sensor *sensor = sensorManager->getSensor(sensorType);
    sensor->loop();
    if (!sensor->isReading())
    {
        if (sensor->isError())
        {
            Serial.print(F("Error reading sensor ")); Serial.println(static_cast<int>(sensorType));
        }
        else
        {
            Serial.print(F("Sensor ")); Serial.print(static_cast<int>(sensorType)); Serial.print(F(" = ")); Serial.println(sensor->getReading());
        }
        sensorManager->setState(nextState);
    }
}
