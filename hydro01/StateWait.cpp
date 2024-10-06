
#include "StateWait.h"
#include "SensorManager.h"

StateWait::StateWait(SensorManager* sensorManager, uint32_t waitTime, State *nextState)
    : State(sensorManager, nextState)
{
    this->waitTime = waitTime;
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
