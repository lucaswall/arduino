
#include "SensorManager.h"
#include "SensorTemperature.h"
#include "SensorTds.h"
#include "SensorPh.h"
#include "SensorLevel.h"
#include "State.h"

void SensorManager::loop()
{
    if (currentState != nullptr) currentState->loop();
}

void SensorManager::setState(State* state)
{
    currentState = state;
    if (currentState != nullptr) currentState->enter();
}
