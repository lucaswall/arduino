
#include "SensorReader.h"
#include "State.h"

void SensorReader::loop()
{
    if (currentState != nullptr) currentState->loop();
}

void SensorReader::setState(State* state)
{
    currentState = state;
    if (currentState != nullptr) currentState->enter();
}
