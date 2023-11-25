#pragma once

#include <Arduino.h>

class XmasLights {

    public:
        XmasLights(uint8_t switchPin);
        void init();
        void on();
        void off();
        void loop(unsigned long dt);
        void setPattern(int *pat);
        void clearPattern();

private:
    uint8_t switchPin;
    int *pattern;
    bool lightOn;
    int patternPos;
    int patternTime;

};
