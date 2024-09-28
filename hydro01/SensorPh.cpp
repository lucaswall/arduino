
#include "SensorPh.h";
#include "SensorManager.h";
#include "config.h";

SensorPh::SensorPh(int pin, int readCount, int readDelay)
    : SensorVoltage(pin, readCount, readDelay)
{
}

float SensorPh::calculateValue(SensorManager* sensorManager, float voltage)
{
    return PhmidPH + (voltage - PhmidV) * ((PhmidPH - PhlowPH) / (PhmidV - PhlowV)) + 0.03 * (sensorManager->getTemperature() - 25.0);
}
