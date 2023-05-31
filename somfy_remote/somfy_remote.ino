
#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SomfyRemote.h>

#include "secrets.h"

#define EMITTER_GPIO D1
#define EEPROM_ADDRESS 0
#define REMOTE REMOTE_BASE
#define CC1101_FREQUENCY 433.42

EEPROMRollingCodeStorage rollingCodeStorage(EEPROM_ADDRESS);
SomfyRemote somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage);

void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println("begin");

    if (ELECHOUSE_cc1101.getCC1101())
    { // Check the CC1101 Spi connection.
        Serial.println("CC1101 SPI Connection OK");
    }
    else
    {
        Serial.println("CC1101 SPI Connection Error");
    }

    ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
    ELECHOUSE_cc1101.setGDO0(EMITTER_GPIO); // set lib internal gdo pin (gdo0). Gdo2 not use for this example.
    // ELECHOUSE_cc1101.setCCMode(1);            // set config for internal transmission mode.
    // ELECHOUSE_cc1101.setModulation(2);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    ELECHOUSE_cc1101.setMHZ(CC1101_FREQUENCY); // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    // ELECHOUSE_cc1101.setSyncMode(2);  // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
    // ELECHOUSE_cc1101.setPA(10);      // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
    // ELECHOUSE_cc1101.setCrc(1);     // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
    Serial.println("Tx Mode");

    somfyRemote.setup();

#if defined(ESP32)
    if (!EEPROM.begin(4))
    {
        Serial.println("failed to initialise EEPROM");
        delay(1000);
    }
#elif defined(ESP8266)
    EEPROM.begin(4);
#endif
}

void sendCC1101Command(Command command)
{
    ELECHOUSE_cc1101.SetTx();
    somfyRemote.sendCommand(command);
    ELECHOUSE_cc1101.setSidle();
}

void loop()
{
    if (Serial.available() > 0)
    {
        const String string = Serial.readStringUntil('\n');
        const Command command = getSomfyCommand(string);
        sendCC1101Command(command);
#ifdef DEBUG
        Serial.println("finished sending");
#endif
    }
}
