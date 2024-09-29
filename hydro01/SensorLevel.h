#pragma once

#include "SensorVoltage.h"

class SensorLevel : SensorVoltage
{

protected:

    virtual float calculateValue(float voltage) override;

};
