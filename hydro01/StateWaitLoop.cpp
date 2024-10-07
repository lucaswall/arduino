
#include "StateWaitLoop.h"

StateWaitLoop::StateWaitLoop(SensorManager* sensorManager, const char *name, uint32_t waitTimeNormal, uint32_t waitTimeFast)
    : StateWait(sensorManager, name, waitTimeNormal)
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
