#pragma once

class SwitchSensor
{

public:

    SwitchSensor(int pin);
    void init();
    void on();
    void off();

protected:

    int pin;

};
