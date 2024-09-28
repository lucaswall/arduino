
#include "StateWait.h"
#include "SensorReader.h"

StateWait::StateWait(SensorReader* sensorReader, uint32_t waitTime, State *nextState)
    : State(sensorReader)
{
    this->waitTime = waitTime;
    this->nextState = nextState;
}

void StateWait::enter()
{
    exitTime = millis() + waitTime;
}

void StateWait::loop()
{
    if (millis() > exitTime)
    {
        sensorReader->setState(nextState);
    }
}
