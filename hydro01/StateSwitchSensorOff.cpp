
#include "StateSwitchSensorOff.h"
#include "SensorManager.h"

StateSwitchSensorOff::StateSwitchSensorOff(SensorManager *sensorManager, SwitchSensor *switchSensor, State *nextState)
    : State(sensorManager, nextState)
{
    this->switchSensor = switchSensor;
}

void StateSwitchSensorOff::enter()
{
    switchSensor->off();
    sensorManager->setState(nextState);
}
