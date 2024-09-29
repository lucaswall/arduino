#pragma once

#include "SensorVoltage.h"

class SensorPh : public SensorVoltage
{

protected:

    virtual float calculateValue(float voltage) override;

};
