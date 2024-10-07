#pragma once

#include "StateWait.h"

class StateWaitLoop : public StateWait
{

public:

    StateWaitLoop(SensorManager* sensorManager, const char *name, uint32_t waitTimeNormal, uint32_t waitTimeFast);

    void timeNormal();
    void timeFast();
    inline bool isFast() const { return fast; }

protected:

    bool fast = false;
    uint32_t waitTimeNormal = 0;
    uint32_t waitTimeFast = 0;

};
