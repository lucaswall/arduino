
#include "StateSensorFinalReport.h"
#include "SensorManager.h"

StateSensorFinalReport::StateSensorFinalReport(SensorManager* sensorManager, State *nextState)
    : State(sensorManager)
{
    this->nextState = nextState;
}

void StateSensorFinalReport::enter()
{
    Serial.println(F("StateSensorFinalReport::enter"));
    Serial.print(F("Temperature = ")); Serial.println(sensorManager->getTemperature());
    Serial.print(F("TDS = ")); Serial.println(sensorManager->getTds());
    Serial.print(F("PH = ")); Serial.println(sensorManager->getPh());
    Serial.print(F("Level = ")); Serial.println(sensorManager->getLevel());
    sensorManager->setState(nextState);
}
