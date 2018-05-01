#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>

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


int max0, prev, in , freq0;
int prevEdge, currentEdge;

int countData = 0, countWave = 0;
int offsetWave = 4;

unsigned long frameTime = 500000;
unsigned long currentTime = 0;
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

  if (in < prev + 30 && in > prev - 30)return NO_EDGE;
  if (prev > in)
    return FALLING_EDGE;
  else if (prev == in)
    return NO_EDGE;
  else return RISING_EDGE;
}


int demodulate2(int numWaveInput) {
  for (int i = 0; i < 4; i++) {
    if ( numWaveInput >= numWaveMap[i] - 4 && numWaveInput < numWaveMap[i] + 4) {
       Serial.print("[DEMODULATE] countWave ");
       Serial.print(numWaveInput);
       Serial.print(" ");
      return i;
    }
  }
  Serial.print("[DEMODULATE_err] countWave ");
  Serial.println(numWaveInput);
  return 4;
}
void onReceiveByte(int inByte){
  Serial.print("BYTE ");
  Serial.println(inByte);
}
int data = 0;
void loop() {

  in = analogRead(A0);
  in += bias;
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

  if ((prevEdge == RISING_EDGE ) && currentEdge == FALLING_EDGE ) {


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

  if ((prevEdge == FALLING_EDGE || prevEdge == NO_EDGE) && currentEdge == FALLING_EDGE && isMin(in)  && isStart) {



    //    Serial.print("CountWave ");
    //    Serial.println(countWave);
    isStart = 0;
  }

  if (isStartCountWave && currentTime - startTime > frameTime) {
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
  prev = in;
  //  delayMicroseconds(1);
}
