
#include "SensorLevel.h"
#include "SensorManager.h"
#include "config.h"

float SensorLevel::calculateValue(float voltage)
{
    return voltage >= minLevelV ? 0.0 : (voltage <= maxLevelV ? 1.0 : (voltage - minLevelV) / (maxLevelV - minLevelV));
}
