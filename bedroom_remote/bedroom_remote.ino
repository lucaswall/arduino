
//#define DEBUG
#define DECODE_NEC

#include <IRremote.hpp>

#define IR_RECEIVE_PIN D5
#define LOOP_DELAY 100 // general loop final delay in ms

void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("Booting"));
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    printActiveIRProtocols(&Serial);
    Serial.println(IR_RECEIVE_PIN);
}

void loop()
{
    if (IrReceiver.decode())
    {
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
        IrReceiver.printIRResultShort(&Serial);                       // Print complete received data in one line
        IrReceiver.printIRSendUsage(&Serial);                         // Print the statement required to send this data
        if (IrReceiver.decodedIRData.protocol == UNKNOWN)
        {
            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            // We have an unknown protocol here, print more info
            IrReceiver.printIRResultRawFormatted(&Serial, true);
        }
        IrReceiver.resume(); // Enable receiving of the next value
        Serial.println();
    }

    delay(LOOP_DELAY);
}
