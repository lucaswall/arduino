
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
#include "StateWaitLoop.h"
#include "MqttDevice.h"

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
    setState(new StateWait(this, "WAIT_INIT", WAIT_INIT))
        ->setNextState(loopState = new StateReadSensor(this, "READ_TEMP", SensorType::TEMPERATURE))
        ->setNextState(new StateReadSensor(this, "READ_LEVEL", SensorType::LEVEL))
        ->setNextState(new StateSwitchSensorOn(this, "ON_PH", &switchPh))
        ->setNextState(new StateWait(this, "SETUP_PH", WAIT_SETUPSENSOR))
        ->setNextState(new StateReadSensor(this, "READ_PH", SensorType::PH))
        ->setNextState(new StateSwitchSensorOff(this, "OFF_PH", &switchPh))
        ->setNextState(new StateWait(this, "CHANGE_SENSOR", WAIT_CHANGESENSOR))
        ->setNextState(new StateSwitchSensorOn(this, "ON_TDS", &switchTds))
        ->setNextState(new StateWait(this, "SETUP_TDS", WAIT_SETUPSENSOR))
        ->setNextState(new StateReadSensor(this, "READ_TDS", SensorType::TDS))
        ->setNextState(new StateSwitchSensorOff(this, "OFF_TDS", &switchTds))
        ->setNextState(new StateSensorFinalReport(this, "FINAL_REPORT", mqttDevice))
        ->setNextState(waitLoop = new StateWaitLoop(this, "WAIT_LOOP", WAIT_LOOP, WAIT_LOOPFAST))
        ->setNextState(loopState);

    lastStatus = millis();
    this->mqttDevice = mqttDevice;
}

void SensorManager::loop()
{
    if (currentState != nullptr) {
        currentState->loop();
        const unsigned long m = millis();
        if (m < lastStatus || m - lastStatus > 1000)
        {
            lastStatus = millis();
            if (mqttDevice != nullptr) mqttDevice->updateStatus(currentState->getStatus());
        }
    }
}

State *SensorManager::setState(State* state)
{
    currentState = state;
    if (currentState != nullptr) currentState->enter();
    return state;
}

void SensorManager::fastReadSensors(bool fastRead)
{
    Serial.print(F("SensorManager::fastReadSensors ")); Serial.println(fastRead);
    if (waitLoop != nullptr)
    {
        if (fastRead) waitLoop->timeFast();
        else waitLoop->timeNormal();
    }
}

bool SensorManager::isFastRead() const
{
    return waitLoop != nullptr && waitLoop->isFast();
}
