
#include "SensorManager.h"

SensorManager sensorManager;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("."));
    delay(2000);
    Serial.println(F("======================= BEGIN ======================="));
    sensorManager.init();
}

void loop()
{
    sensorManager.loop();
}
