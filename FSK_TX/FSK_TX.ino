#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>

#define defaultFreq 1700
#define f0 500
#define f1 750
#define f2 1000
#define f3 1250

const int SAMPLING = 32;
uint16_t S_DAC[SAMPLING] = {2048, 4095, 2048, 0};
unsigned long frameTime = 510000 + 50000;

Adafruit_MCP4725 dac;

void setup()
{
  // put your setup code here, to run once:
  dac.begin(0x64);


  Serial.begin(115200);
  Serial.flush();

  dac.setVoltage(0, false);

  for (int i = 0; i < SAMPLING; i++)
  {
    S_DAC[i] = (sin(2 * PI / SAMPLING * i) + 1) * 2047;
  }
}
void generateWave(int freqency)
{

  int numWave = freqency / 250 * 8;

  int delayTime = (10000000 / freqency - 1 / defaultFreq) / SAMPLING;

  unsigned long elapsedTime = micros();
  for (int i = 0; i < numWave; i++)
  {
    for (int j = 0; j < SAMPLING; j++)
    {

      dac.setVoltage(S_DAC[j], false);
      delayMicroseconds(delayTime);
    }
  }

  long waitForFullFrame = frameTime - (micros() - elapsedTime);

  while (waitForFullFrame > 0)
  {
    if (waitForFullFrame > 16000)
    {
      delayMicroseconds(16000);
      waitForFullFrame -= 16000;
    }
    else
    {
      delayMicroseconds(waitForFullFrame);
      break;
    }
  }
}

void sendByte(int inByte)
{

  Serial.print(inByte);
  Serial.print(" -> ");
  int input[4];
  for (int i = 3; i >= 0; i--)
  {
    input[i] = inByte & 3;
    Serial.print(input[i], BIN);
    Serial.print(" ");
    inByte >>= 2;
  }
  Serial.println();
  for (int k = 3; k >= 0; k--)
  {
    if (input[k] == 0)
    {
      generateWave(f0);
    }
    else if (input[k] == 1)
    {
      generateWave(f1);
    }
    else if (input[k] == 2)
    {
      generateWave(f2);
    }
    else if (input[k] == 3)
    {
      generateWave(f3);
    }
  }

  delayMicroseconds(5000);
}
int in = 0;

void loop()
{
  if (Serial.available() > 0)
  {
    in = Serial.parseInt();
    if (in == 0)
      return;
    sendByte(in);
  }
}
