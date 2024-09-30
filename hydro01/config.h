#pragma once

#include <Arduino.h>

#define VREF 3.3
#define MAXADC 4095.0

#define SensorReadCount 30
#define SensorReadDelay 10

#define RefTemperature 25.0
#define TdsFactor 0.5

#define PhlowPH 4.01
#define PhlowV 2.00
#define PhmidPH 6.86
#define PhmidV 1.66

#define minLevelV 2.72
#define maxLevelV 0.55

#define SensorTempPin 26
#define SensorTdsPin 39
#define SensorPhPin 36
#define SensorWaterLevel 34

#define SwitchTdsPin 4
#define SwitchPhPin 5

enum class SensorType
{
    TEMPERATURE,
    TDS,
    PH,
    LEVEL,
    SENSOR_COUNT
};
