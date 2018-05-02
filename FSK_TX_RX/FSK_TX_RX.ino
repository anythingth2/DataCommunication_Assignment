#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>

#define defaultFreq 1700
#define f0 500
#define f1 750
#define f2 1000
#define f3 1250

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#define bias 0
#define MAX 1023
#define MIN 0+bias
#define RISING_EDGE 1
#define FALLING_EDGE 2
#define NO_EDGE 3
#define IDLE_EDGE 4
#define IDLE_VALUE 50


int numWaveMap[] = {16, 24, 32, 40};

int max0, prev, inRX , freq0;
int prevEdge, currentEdge;

int countData = 0, countWave = 0;
int offsetWave = 0;

const int SAMPLING = 4;
uint16_t S_DAC[SAMPLING] = {2048, 4095, 2048, 0};
unsigned long frameTime = 360228 + 15000;

unsigned long frameTimeRX =  89000 * 4 ;
unsigned long currentTime = 0;
unsigned long startTime = 0;
int isStart = 0;
int isStartCountWave = 0;

Adafruit_MCP4725 dac;

void setup()
{
  // put your setup code here, to run once:
  dac.begin(0x64);

  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);

  Serial.begin(115200);
  Serial.flush();

  dac.setVoltage(0, false);

  for (int i = 0; i < SAMPLING; i++)
  {
    S_DAC[i] = (sin(2 * PI / SAMPLING * i) + 1) * 2047;
  }

  prevEdge = FALLING_EDGE;
  startTime = micros();

}

int isMax(int value) {
  return value > MAX - 5 && value < MAX + 5;
}
int isMin(int value) {
  return value < MIN + 5;
}


int demodulate2(int numWaveInput) {
  for (int i = 0; i < 4; i++) {
    if ( numWaveInput >= numWaveMap[i] - 4 && numWaveInput < numWaveMap[i] + 4) {
      //       Serial.print("[DEMODULATE] countWave ");
      //       Serial.print(numWaveInput);
      //       Serial.print(" ");
      return i;
    }
  }
  Serial.print("[DEMODULATE_err] countWave ");
  Serial.println(numWaveInput);
  return 4;
}
void onReceiveByte(int inByte) {
  Serial.print("BYTE ");
  Serial.println(inByte);
}

int data = 0;

int getEdge() {

  if (inRX < prev + 10 && inRX > prev - 10)return NO_EDGE;
  if (prev > inRX)
    return FALLING_EDGE;
  else if (prev == inRX)
    return NO_EDGE;
  else return RISING_EDGE;
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
  inRX = analogRead(A0);
  inRX += bias;
  currentEdge = getEdge();
  currentTime = micros();

  if (!isStartCountWave && currentEdge == RISING_EDGE)
  {
    isStartCountWave = true;
    startTime = currentTime;
    // Serial.print("Start_Counting_Wave at ");
    // Serial.println(in);
  }

  if (isStartCountWave) {
    delayMicroseconds(1600);
    //        unsigned long ts = micros();
    //        Serial.print("in ");
    //        Serial.print(in);
    //        Serial.print("\tedge ");
    //        switch (currentEdge) {
    //          case RISING_EDGE: Serial.print("RISING"); break;
    //          case FALLING_EDGE: Serial.print("FALLING"); break;
    //          case NO_EDGE: Serial.print("NO_EDGE"); break;
    //        }
    //        Serial.println();
    //
    //
  }

  if ((prevEdge == RISING_EDGE || prevEdge == NO_EDGE) && currentEdge == FALLING_EDGE &&isMax(prev) ) {


    isStart = 1;
    countWave++;
    //    Serial.print("CountWave ");
    //    Serial.println(countWave);
    //    Serial.print("\tcountData ");
    //    Serial.print(countData);

    //    Serial.print("\tFreq ");
    //    Serial.print(freq0);
    //    Serial.println();
  }

  if ((prevEdge == FALLING_EDGE || prevEdge == NO_EDGE) && currentEdge == FALLING_EDGE && isMin(inRX)  && isStart) {



    //    Serial.print("CountWave ");
    //    Serial.println(countWave);
    isStart = 0;
  }

  if (isStartCountWave && currentTime - startTime > frameTimeRX) {
    int dataDemod = demodulate2(countWave + offsetWave);
    data +=  dataDemod << (countData * 2);

    Serial.print(dataDemod, BIN);
    Serial.print(" ");
    if (++countData == 4) {

      onReceiveByte(data);

      Serial.print("\t -> ");
      Serial.print(data);
      Serial.println();
      countData = 0;
      data = 0;
      delayMicroseconds(5000);
    }
    startTime = currentTime;
    countWave = 0;
    isStartCountWave = false;
    //      Serial.println("End_Counting_wave");
  }

  prevEdge = currentEdge;
  prev = inRX;

  //Delay between arduino and DAC is 588 <<-------##########
  if (Serial.available() > 0)
  {
    in = Serial.parseInt();
    if (in == 0)
      return;
    sendByte(in);
  }
}
