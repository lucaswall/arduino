
#include "StateSwitchSensorOn.h"
#include "SensorManager.h"

StateSwitchSensorOn::StateSwitchSensorOn(SensorManager *sensorManager, SwitchSensor *switchSensor, State *nextState)
    : State(sensorManager, nextState)
{
    this->switchSensor = switchSensor;
}

void StateSwitchSensorOn::enter()
{
    switchSensor->on();
    sensorManager->setState(nextState);
}
