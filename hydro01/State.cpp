
#include "State.h";

State::State(SensorManager* sensorManager, const char *name)
{
    this->sensorManager = sensorManager;
    this->name = name;
}
