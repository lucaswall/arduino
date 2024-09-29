
#include "SensorPh.h"
#include "SensorManager.h"
#include "config.h"

float SensorPh::calculateValue(float voltage)
{
    return PhmidPH + (voltage - PhmidV) * ((PhmidPH - PhlowPH) / (PhmidV - PhlowV)) + 0.03 * (sensorManager->getTemperature() - 25.0);
}
