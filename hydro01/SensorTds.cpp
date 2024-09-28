
#include "SensorTds.h";
#include "SensorManager.h";
#include "config.h";

SensorTds::SensorTds(int pin, int readCount, int readDelay)
    : SensorVoltage(pin, readCount, readDelay)
{
}

float SensorTds::calculateValue(SensorManager* sensorManager, float voltage)
{
    const float compC = 1.0 + 0.02 * (sensorManager->getTemperature() - RefTemperature);
    const float compV = voltage / compC;
    return (133.42 * compV * compV * compV - 255.86 * compV * compV + 857.39 * compV) * TdsFactor;
}
