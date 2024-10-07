
#include "StateSwitchSensorOn.h"
#include "SensorManager.h"

StateSwitchSensorOn::StateSwitchSensorOn(SensorManager *sensorManager, const char *name, SwitchSensor *switchSensor)
    : State(sensorManager, name)
{
    this->switchSensor = switchSensor;
}

void StateSwitchSensorOn::enter()
{
    switchSensor->on();
    sensorManager->setState(nextState);
}
