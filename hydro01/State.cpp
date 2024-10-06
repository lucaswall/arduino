
#include "State.h";

State::State(SensorManager* sensorManager, State *nextState)
{
    this->sensorManager = sensorManager;
    this->nextState = nextState;
}
