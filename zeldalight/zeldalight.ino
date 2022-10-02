
#include "DHT.h"

#define LIGHT_WHITE D3
#define LIGHT_RED D4
#define LIGHT_GREEN D5
#define LIGHT_BLUE D6

int lightWhite = 0;
int lightRed = 0;
int lightGreen = 0;
int lightBlue = 0;

DHT dht(D8, DHT21);

void setup() {

  delay(1000);
  Serial.begin(115200);
  Serial.println(F("!"));
  delay(1000);
  Serial.println(F("Booting"));

  pinMode(D8, INPUT);
  dht.begin();
  
  pinMode(LIGHT_WHITE, OUTPUT);
  pinMode(LIGHT_RED, OUTPUT);
  pinMode(LIGHT_GREEN, OUTPUT);
  pinMode(LIGHT_BLUE, OUTPUT);
  digitalWrite(LIGHT_WHITE, LOW);
  digitalWrite(LIGHT_RED, HIGH);
  digitalWrite(LIGHT_GREEN, HIGH);
  digitalWrite(LIGHT_BLUE, HIGH);
}

int readDelay = 10;

void loop() {

  lightWhite += 20;
  if (lightWhite > 1023) lightWhite = 0;
  analogWrite(LIGHT_WHITE, lightWhite);

  lightRed -= 1;
  if (lightRed < 0) lightRed = 250;
  analogWrite(LIGHT_RED, lightRed);

  lightGreen -= 2;
  if (lightGreen < 0) lightGreen = 250;
  analogWrite(LIGHT_GREEN, lightGreen);

  lightBlue -= 3;
  if (lightBlue < 0) lightBlue = 250;
  analogWrite(LIGHT_BLUE, lightBlue);

  if (--readDelay < 0) {
    readDelay = 10;
    const float h = dht.readHumidity();
    const float t = dht.readTemperature();
    const float f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.print(h);
      Serial.print(F(" "));
      Serial.print(t);
      Serial.print(F(" "));
      Serial.println(f);
      Serial.println("Failed to read from DHT sensor!");
    } else {
      const float hif = dht.computeHeatIndex(f, h);
      const float hic = dht.computeHeatIndex(t, h, false);
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F(" %\t"));
      Serial.print(F("Temperature: "));
      Serial.print(t);
      Serial.print(F(" *C "));
      Serial.print(f);
      Serial.print(F(" *F\t"));
      Serial.print(F("Heat index: "));
      Serial.print(hic);
      Serial.print(F(" *C "));
      Serial.print(hif);
      Serial.println(F(" *F"));
    }
  }

  delay(100);

}
