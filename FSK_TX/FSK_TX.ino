#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>

#define defaultFreq 1700
#define f0 500
#define f1 750
#define f2 1000
#define f3 1250
//#define f0 500
//#define f1 1500
//#define f2 2500
//#define f3 3500

int delay0, delay1, delay2, delay3;
const int SAMPLING = 4;
uint16_t S_DAC[SAMPLING] = {2048, 4095, 2048, 0};
unsigned long frameTime = 360228 + 15000;
Adafruit_MCP4725 dac;

void setup() {
  // put your setup code here, to run once:
  dac.begin(0x64);

  delay0 = 1000000 / 50  - defaultFreq;
  delay1 = 1000000 / 75  - defaultFreq;
  delay2 = 1000000 / 100  - defaultFreq;
  delay3 = 1000000 / 125 - defaultFreq;



  Serial.begin(115200);
  Serial.flush();

  Serial.print("delay[0-3] -> ");
  Serial.print(delay0);
  Serial.print(" ");
  Serial.print(delay1);
  Serial.print(" ");
  Serial.print(delay2);
  Serial.print(" ");
  Serial.print(delay3);
  Serial.println();
  dac.setVoltage(0, false);

  for (int i = 0; i < SAMPLING; i++) {
    S_DAC[i] = (sin(2 * PI / SAMPLING * i ) + 1) * 2047 ;
    //    Serial.print("S_DAC = ");
    //    Serial.println(S_DAC[i]);
  }
}
void generateWave(int freqency) {
  //  Serial.print("Start Generate Wave ");
  //  Serial.print(freqency);
  int numWave =  freqency / 250 * 8;
  //  numWave = 1;
  int delayTime = (10000000 / freqency  - 1 / defaultFreq) / SAMPLING ;
  //  delayTime /=10;
  //  Serial.print("delay ");

  //  Serial.print("\tdelayTime ");
  //  Serial.print(delayTime);
  Serial.print("\tnumWave ");
  Serial.println(numWave);
  Serial.print("elapsedTime : ");
  unsigned long elapsedTime = micros();
  for (int i = 0; i < numWave; i++) {
    for (int j = 0; j < SAMPLING; j++) {

      dac.setVoltage(S_DAC[j], false);
      delayMicroseconds(delayTime);

    }
  }


  unsigned long waitForFullFrame = frameTime - (micros() - elapsedTime);
  //  if (waitForFullFrame > 0)
  while (waitForFullFrame > 0) {
    if (waitForFullFrame > 16000) {
      delayMicroseconds(16000);
      waitForFullFrame -= 16000;
    } else {
      delayMicroseconds(waitForFullFrame); break;
    }

  }
  elapsedTime = micros() - elapsedTime;
  Serial.println(elapsedTime);
  //  Serial.println("\t Generated!");
}
int input[4];
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    int in = Serial.parseInt();
    //    Serial.write(in);
    if (in == 0)return;
    Serial.print(in);
    Serial.print(" -> ");

    for (int i = 3; i >= 0; i--) {
      input[i] = in & 3;
      Serial.print(input[i], BIN);
      Serial.print(" ");
      in >>= 2;
    }
    Serial.println();
    for (int k = 3; k >= 0; k--) {
      if (input[k] == 0) {
        generateWave(f0);
      } else if (input[k] == 1) {
        generateWave(f1);
      } else if (input[k] == 2) {
        generateWave(f2);
      } else if (input[k] == 3) {
        generateWave(f3);
      }
    }

    dac.setVoltage(0, false);
  }
}
