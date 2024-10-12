
#include "StateSwitchSensorOff.h"
#include "SensorManager.h"

StateSwitchSensorOff::StateSwitchSensorOff(SensorManager *sensorManager, const char *name, SwitchSensor *switchSensor)
    : State(sensorManager, name)
{
    this->switchSensor = switchSensor;
}

void StateSwitchSensorOff::enter()
{
    status = name + ": switch sensor off";
    switchSensor->off();
    sensorManager->setState(nextState);
}
