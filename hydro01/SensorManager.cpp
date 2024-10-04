
#include "SensorManager.h"
#include "SensorTemperature.h"
#include "SensorTds.h"
#include "SensorPh.h"
#include "SensorLevel.h"
#include "State.h"
#include "StateReadSensor.h"
#include "StateWait.h"
#include "StateSwitchSensorOn.h"
#include "StateSwitchSensorOff.h"
#include "StateSensorFinalReport.h"

SensorManager::SensorManager()
        : switchPh(SwitchPhPin), switchTds(SwitchTdsPin)
{
    sensors[static_cast<int>(SensorType::TEMPERATURE)] = new SensorTemperature(SensorTempPin);
    sensors[static_cast<int>(SensorType::TDS)] = new SensorTds(SensorTdsPin, SensorReadCount, SensorReadDelay);
    sensors[static_cast<int>(SensorType::PH)] = new SensorPh(SensorPhPin, SensorReadCount, SensorReadDelay);
    sensors[static_cast<int>(SensorType::LEVEL)] = new SensorLevel(SensorWaterLevel, SensorReadCount, SensorReadDelay);
}

void SensorManager::init(MqttDevice *mqttDevice)
{
    Serial.println(F("SensorManager::init"));
    for (int i = 0; i < static_cast<int>(SensorType::SENSOR_COUNT); i++)
    {
        sensors[i]->init(this);
    }
    switchPh.init();
    switchTds.init();

    StateWait *stateWaitEndLoop = new StateWait(this, 10000, nullptr);
    StateSensorFinalReport *stateSensorFinalReport = new StateSensorFinalReport(this, mqttDevice, stateWaitEndLoop);
    StateSwitchSensorOff *stateSwitchTdsOff = new StateSwitchSensorOff(this, &switchTds, stateSensorFinalReport);
    StateReadSensor *stateReadTds = new StateReadSensor(this, SensorType::TDS, stateSwitchTdsOff);
    StateWait *stateWaitTdsSetup = new StateWait(this, 5000, stateReadTds);
    StateSwitchSensorOn *stateSwitchTdsOn = new StateSwitchSensorOn(this, &switchTds, stateWaitTdsSetup);
    StateWait *stateWaitPhToTds = new StateWait(this, 5000, stateSwitchTdsOn);
    StateSwitchSensorOff *stateSwitchPhOff = new StateSwitchSensorOff(this, &switchPh, stateWaitPhToTds);
    StateReadSensor *stateReadPh = new StateReadSensor(this, SensorType::PH, stateSwitchPhOff);
    StateWait *stateWaitPhSetup = new StateWait(this, 5000, stateReadPh);
    StateSwitchSensorOn *stateSwitchPhOn = new StateSwitchSensorOn(this, &switchPh, stateWaitPhSetup);
    StateReadSensor *stateReadLevel = new StateReadSensor(this, SensorType::LEVEL, stateSwitchPhOn);
    StateReadSensor *stateReadTemperature = new StateReadSensor(this, SensorType::TEMPERATURE, stateReadLevel);
    stateWaitEndLoop->setNextState(stateReadTemperature);
    StateWait *stateWaitInitial = new StateWait(this, 10000, stateReadTemperature);
    setState(stateWaitInitial);
}

void SensorManager::loop()
{
    if (currentState != nullptr) currentState->loop();
}

void SensorManager::setState(State* state)
{
    currentState = state;
    if (currentState != nullptr) currentState->enter();
}
