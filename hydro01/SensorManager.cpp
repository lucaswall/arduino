
#include "SensorManager.h"
#include "SensorTemperature.h"
#include "SensorTds.h"
#include "SensorPh.h"
#include "SensorLevel.h"
#include "State.h"

SensorManager::SensorManager()
        : switchPh(SwitchPhPin), switchTds(SwitchTdsPin)
{
    sensors[static_cast<int>(SensorType::TEMPERATURE)] = new SensorTemperature(SensorTempPin);
    sensors[static_cast<int>(SensorType::TDS)] = new SensorTds(SensorTdsPin, SensorReadCount, SensorReadDelay);
    sensors[static_cast<int>(SensorType::PH)] = new SensorPh(SensorPhPin, SensorReadCount, SensorReadDelay);
    sensors[static_cast<int>(SensorType::LEVEL)] = new SensorLevel(SensorWaterLevel, SensorReadCount, SensorReadDelay);
}

void SensorManager::init()
{
    for (int i = 0; i < static_cast<int>(SensorType::SENSOR_COUNT); i++)
    {
        sensors[i]->init(this);
    }
    switchPh.init();
    switchTds.init();
}

void SensorManager::loop()
{
    for (int i = 0; i < static_cast<int>(SensorType::SENSOR_COUNT); i++)
    {
        sensors[i]->loop();
    }
    if (currentState != nullptr) currentState->loop();
}

void SensorManager::setState(State* state)
{
    currentState = state;
    if (currentState != nullptr) currentState->enter();
}
