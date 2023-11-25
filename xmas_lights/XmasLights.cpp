
#include "XmasLights.h"

XmasLights::XmasLights(uint8_t switchPinP) {
    switchPin = switchPinP;
    pattern = nullptr;
}

void XmasLights::init() {
    pinMode(switchPin, OUTPUT);
    off();
}

void XmasLights::on() {
    digitalWrite(switchPin, HIGH);
    lightOn = true;
}

void XmasLights::off() {
    digitalWrite(switchPin, LOW);
    lightOn = false;
}

void XmasLights::loop(unsigned long dt) {
    if (pattern) {
        patternTime -= dt;
        while (patternTime <= 0) {
            patternPos++;
            if (pattern[patternPos] < 0) patternPos = 0;
            lightOn = !lightOn;
            patternTime += pattern[patternPos];
        }
        if (lightOn) on(); else off();
    }
}

void XmasLights::setPattern(int *pat) {
    if (pat[0] < 0) {
        clearPattern();
        return;
    }
    pattern = pat;
    patternPos = 0;
    lightOn = true;
    patternTime = pattern[0];
    on();
}

void XmasLights::clearPattern() {
    pattern = nullptr;
    off();
}
