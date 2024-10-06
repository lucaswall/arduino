
#include "StateWaitLoop.h"

StateWaitLoop::StateWaitLoop(SensorManager* sensorManager, uint32_t waitTimeNormal, uint32_t waitTimeFast, State *nextState)
    : StateWait(sensorManager, waitTimeNormal, nextState)
{
    this->waitTimeNormal = waitTimeNormal;
    this->waitTimeFast = waitTimeFast;
}

void StateWaitLoop::timeNormal()
{
    Serial.println(F("StateWaitLoop::timeNormal"));
    fast = false;
    exitTime = millis() + waitTimeNormal;
    waitTime = waitTimeNormal;
}

void StateWaitLoop::timeFast()
{
    Serial.println(F("StateWaitLoop::timeFast"));
    fast = true;
    exitTime = millis() + waitTimeFast;
    waitTime = waitTimeFast;
}
