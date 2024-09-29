#pragma once

#include <Arduino.h>

const float VREF = 3.3;
const float MAXADC = 4095.0;

const float RefTemperature = 25.0;
const float TdsFactor = 0.5;

const float PhlowPH = 4.01;
const float PhlowV = 2.00;
const float PhmidPH = 6.86;
const float PhmidV = 1.66;

const float minLevelV = 2.72;
const float maxLevelV = 0.55;

const uint8_t SensorTempPin = 26;
const uint8_t SensorTdsPin = 39;
const uint8_t SensorPhPin = 36;
const uint8_t SensorWaterLevel = 34;

const uint8_t SwitchTdsPin = 4;
const uint8_t SwitchPhPin = 5;
