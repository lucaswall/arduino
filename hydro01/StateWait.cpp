
#include "StateWait.h"
#include "SensorManager.h"

StateWait::StateWait(SensorManager* sensorManager, uint32_t waitTime, State *nextState)
    : State(sensorManager)
{
    this->waitTime = waitTime;
    this->nextState = nextState;
}

void StateWait::enter()
{
    Serial.print(F("Wait for ")); Serial.print(waitTime); Serial.println(F(" ms"));
    exitTime = millis() + waitTime;
}

void StateWait::loop()
{
    if (millis() > exitTime)
    {
        Serial.println(F("Wait done"));
        sensorManager->setState(nextState);
    }
}
