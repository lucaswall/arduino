#pragma once

#include "Sensor.h"
#include "SwitchSensor.h"
#include "config.h"
#include "State.h"

class MqttDevice;
class StateWaitLoop;

class SensorManager
{

public:

    SensorManager();

    void init(MqttDevice *mqttDevice);
    void loop();
    State *setState(State* state);
    void fastReadSensors(bool fastRead);

    inline float getTemperature() const { return sensors[static_cast<int>(SensorType::TEMPERATURE)]->getReading(); }
    inline float getTds() const { return sensors[static_cast<int>(SensorType::TDS)]->getReading(); }
    inline float getPh() const { return sensors[static_cast<int>(SensorType::PH)]->getReading(); }
    inline float getLevel() const { return sensors[static_cast<int>(SensorType::LEVEL)]->getReading(); }
    inline Sensor* getSensor(SensorType sensorType) const { return sensors[static_cast<int>(sensorType)]; }
    bool isFastRead() const;

protected:

    State* currentState = nullptr;
    Sensor* sensors[static_cast<int>(SensorType::SENSOR_COUNT)] = { nullptr };
    SwitchSensor switchPh;
    SwitchSensor switchTds;
    StateWaitLoop *waitLoop = nullptr;
    uint32_t lastStatus = 0;
    MqttDevice *mqttDevice = nullptr;

};
