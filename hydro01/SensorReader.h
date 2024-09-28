#pragma once

class State;

class SensorReader
{

public:

    void loop();
    void setState(State* state);

protected:

    State* currentState = nullptr;

};
