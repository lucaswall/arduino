
#include "StateSwitchSensorOn.h"
#include "SensorManager.h"

StateSwitchSensorOn::StateSwitchSensorOn(SensorManager *sensorManager, SwitchSensor *switchSensor, State *nextState)
    : State(sensorManager)
{
    this->switchSensor = switchSensor;
    this->nextState = nextState;
}

void StateSwitchSensorOn::enter()
{
    switchSensor->on();
    sensorManager->setState(nextState);
}
