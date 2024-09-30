
#include "StateSwitchSensorOff.h"
#include "SensorManager.h"

StateSwitchSensorOff::StateSwitchSensorOff(SensorManager *sensorManager, SwitchSensor *switchSensor, State *nextState)
    : State(sensorManager)
{
    this->switchSensor = switchSensor;
    this->nextState = nextState;
}

void StateSwitchSensorOff::enter()
{
    switchSensor->off();
    sensorManager->setState(nextState);
}
