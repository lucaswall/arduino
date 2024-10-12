
#include "StateWait.h"
#include "SensorManager.h"

StateWait::StateWait(SensorManager* sensorManager, const char *name, uint32_t waitTime)
    : State(sensorManager, name)
{
    this->waitTime = waitTime;
}

void StateWait::enter()
{
    status = name + ": wait for " + String(waitTime/1000) + " s";
    Serial.print(F("Wait for ")); Serial.print(waitTime); Serial.println(F(" ms"));
    exitTime = millis() + waitTime;
    nextStatusTime = millis() + 1000;
}

void StateWait::loop()
{
    const unsigned long m = millis();
    if (m > exitTime)
    {
        Serial.println(F("Wait done"));
        sensorManager->setState(nextState);
    }
    if (m > nextStatusTime)
    {
        nextStatusTime = millis() + 1000;
        status = name + ": wait for " + String((exitTime - millis()) / 1000) + " s";
    }
}
