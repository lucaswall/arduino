
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
}

void XmasLights::off() {
    digitalWrite(switchPin, LOW);
}

void XmasLights::loop(unsigned long dt) {
    if (pattern) {
        patternTime -= dt;
        if (patternTime <= 0) {
            patternPos++;
            if (pattern[patternPos] <= 0) patternPos = 0;
            if (lastOn) off(); else on();
            lastOn = !lastOn;
            patternTime = pattern[patternPos];
        }
    }
}

void XmasLights::setPattern(int *pat) {
    pattern = pat;
    patternPos = 0;
    lastOn = true;
    patternTime = pattern[0];
    on();
}

void XmasLights::clearPattern() {
    pattern = nullptr;
    off();
}
