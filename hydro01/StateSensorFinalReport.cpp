
#include "StateSensorFinalReport.h"
#include "SensorManager.h"
#include "MqttDevice.h"

StateSensorFinalReport::StateSensorFinalReport(SensorManager* sensorManager, MqttDevice *mqttDevice, State *nextState)
    : State(sensorManager, nextState)
{
    this->mqttDevice = mqttDevice;
}

void StateSensorFinalReport::enter()
{
    Serial.println(F("StateSensorFinalReport::enter"));
    Serial.print(F("Temperature = ")); Serial.println(sensorManager->getTemperature());
    Serial.print(F("TDS = ")); Serial.println(sensorManager->getTds());
    Serial.print(F("PH = ")); Serial.println(sensorManager->getPh());
    Serial.print(F("Level = ")); Serial.println(sensorManager->getLevel());
    Serial.print(F("Fast read = ")); Serial.println(sensorManager->isFastRead());
    Sensor *temperature = sensorManager->getSensor(SensorType::TEMPERATURE);
    Sensor *tds = sensorManager->getSensor(SensorType::TDS);
    Sensor *ph = sensorManager->getSensor(SensorType::PH);
    Sensor *level = sensorManager->getSensor(SensorType::LEVEL);
    mqttDevice->updateSensors(temperature, tds, ph, level, sensorManager->isFastRead());
    sensorManager->setState(nextState);
}
