#include <Wire.h>
#include <Adafruit_MCP4725.h>


#define defaultFreq 1700
#define pi 3.142
Adafruit_MCP4725 dac;

const int SAMPLING = 32;
int sinDAC[SAMPLING];
void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  dac.begin(0x64);

  for (int i = 0; i < SAMPLING; i++) {
    sinDAC[i] = 2047 * (sin(pi * i / SAMPLING * 2 ) + 1);
    Serial.println(sinDAC[i]);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
//
//      for (int i = 0; i < SAMPLING; i++) {
//        dac.setVoltage(sinDAC[i], false);
//        delay(100);
//      }
  dac.setVoltage(4000, false);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  dac.setVoltage(0, false);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);

  if (Serial.available() > 0 ) {
    Serial.read();

    for (int i = 0; i < 10; i++) {
      dac.setVoltage(4000, false);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      dac.setVoltage(0, false);
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
    }
  }
}
