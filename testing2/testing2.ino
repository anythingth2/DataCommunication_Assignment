#include <Wire.h>
#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;
void setup() {
  // put your setup code here, to run once:
  dac.begin(0x64);
}

void loop() {
  // put your main code here, to run repeatedly:
//  dac.setVoltage(2048*2-1, false);
//  delay(50);
  dac.setVoltage(4095, false);
  delay(50);
  dac.setVoltage(0, false);
  delay(50);
}
