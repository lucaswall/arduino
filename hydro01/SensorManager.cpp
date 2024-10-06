
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

    State *loopState;
    setState(new StateWait(this, 10000))
        ->setNextState(loopState = new StateReadSensor(this, SensorType::TEMPERATURE))
        ->setNextState(new StateReadSensor(this, SensorType::LEVEL))
        ->setNextState(new StateSwitchSensorOn(this, &switchPh))
        ->setNextState(new StateReadSensor(this, SensorType::PH))
        ->setNextState(new StateSwitchSensorOff(this, &switchPh))
        ->setNextState(new StateWait(this, 5000))
        ->setNextState(new StateSwitchSensorOn(this, &switchTds))
        ->setNextState(new StateReadSensor(this, SensorType::TDS))
        ->setNextState(new StateSwitchSensorOff(this, &switchTds))
        ->setNextState(new StateSensorFinalReport(this, mqttDevice))
        ->setNextState(new StateWait(this, 10000))
        ->setNextState(loopState);

}

void SensorManager::loop()
{
    if (currentState != nullptr) currentState->loop();
}

State *SensorManager::setState(State* state)
{
    currentState = state;
    if (currentState != nullptr) currentState->enter();
    return state;
}
