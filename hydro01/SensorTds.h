#pragma once

#include "SensorVoltage.h"

class SensorTds : public SensorVoltage
{

protected:

    virtual float calculateValue(float voltage) override;
    
};
