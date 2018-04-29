#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define r_slope 512
#define MAX 1023
#define MIN 0
#define RISING_EDGE 1
#define FALLING_EDGE 2
#define NO_EDGE 3

int freqMod[] = {386, 572, 750, 926};
char freqData[][2] = {"00", "01", "10", "11"};
//int numWaveMap[] = {4, 6, 8, 10};
int numWaveMap[] = {16, 24, 32, 40};
int freqMap[] = {};

int max0, prev, in , freq0;
int prevEdge, currentEdge;
unsigned long topWaveTime, bottomWaveTime;
int countData = 0, countWave = 0;
unsigned long frameTime = 89000 * 4 , currentTime = 0;

unsigned long startTime = 0;
int isStart = 0;
int isStartCountWave = 0;

void setup() {
  // put your setup code here, to run once:
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
  Serial.begin(115200);

  prevEdge = FALLING_EDGE;
  startTime = micros();
}

int isMax(int value) {
  return value > MAX - 5 && value < MAX + 5;
}
int isMin(int value) {
  return value < MIN + 5;
}
int getEdge() {

  if (in < prev + 10 && in > prev - 10)return NO_EDGE;

  if (prev > in)
    return FALLING_EDGE;
  else if (prev == in)
    return NO_EDGE;
  else return RISING_EDGE;
}

int demodulate(int freqInput) {
  for (int i = 0; i < 4; i++) {
    if (freqInput > freqMod[i] - 25 && freqInput < freqMod[i] + 25) {
      return i;
    }
  }
  return -1;
}
int demodulate2(int numWaveInput) {
  for (int i = 0; i < 4; i++) {
    if ( numWaveInput >= numWaveMap[i] - 4 && numWaveInput < numWaveMap[i] + 4) {
      Serial.print("DEMODULATE ->");
      Serial.println(numWaveInput);
      return i;
    }
  }


  Serial.print("DEMODULATE_err->");
  Serial.println(numWaveInput);
  return 4;
}
int data = 0;
void loop() {
  // put your main code here, to run repeatedly:
  in = analogRead(A0);
  currentEdge = getEdge();
  currentTime = micros();
  if (!isStartCountWave && currentEdge == RISING_EDGE)
  {
    isStartCountWave = true;
    startTime = currentTime;
    Serial.println("Start_Counting_Wave ");
  }

//    if (isStartCountWave) {
//      Serial.print("in ");
//      Serial.print(in);
//      Serial.print("\tedge ");
//      switch (currentEdge) {
//        case RISING_EDGE: Serial.print("RISING"); break;
//        case FALLING_EDGE: Serial.print("FALLING"); break;
//        case NO_EDGE: Serial.print("NO_EDGE"); break;
//      }
//      Serial.println();
//    }

  if ((prevEdge == RISING_EDGE || prevEdge == NO_EDGE) && currentEdge == FALLING_EDGE && isMax(prev)) {


    topWaveTime = currentTime;
    isStart = 1;
    //    Serial.print("\tcountData ");
    //    Serial.print(countData);

    //    Serial.print("\tFreq ");
    //    Serial.print(freq0);
    //    Serial.println();
  }

  if ((prevEdge == FALLING_EDGE || prevEdge == NO_EDGE) && currentEdge == FALLING_EDGE && isMin(in)  && isStart) {
    bottomWaveTime = currentTime;
    //    freq0 = 1000000 / (bottomWaveTime - topWaveTime) * 2;

    //    Serial.print("CountWave ");
    //    Serial.println(countWave);
    //        Serial.print("Freq ");
    //        Serial.print(freq0);
    //        Serial.print("\t");
    countWave++;
//    Serial.print("CountWave ");
//    Serial.println(countWave);

    isStart = 0;
  }

  if (isStartCountWave && currentTime - startTime > frameTime) {
    int dataDemod = demodulate2(countWave);
    data +=  dataDemod << (countData * 2);

//    Serial.print(dataDemod, BIN);
//    Serial.println(" ");
    if (++countData == 4) {
      Serial.print("\t -> ");
      Serial.print(data);
      Serial.println();
      countData = 0;
      data = 0;
    }
    startTime = currentTime;
    countWave = 0;
    isStartCountWave = false;
    //      Serial.println("End_Counting_wave");
  }
  //
  //  if (in != 0)
  //    Serial.println();

  prevEdge = currentEdge;
  prev = in;
  //  delayMicroseconds(1);
}
