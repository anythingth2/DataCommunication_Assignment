

int max0 = 0, min0 = 1024;
double mean = 0;
int n = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  mean = analogRead(A0);
  n++;
}

void loop() {
  // put your main code here, to run repeatedly:
  int in = analogRead(A0);

  
    Serial.println(in);
    delay(10);
  //  if (in > max0) {
  //    max0 = in;
  //    Serial.print("Max ");
  //    Serial.println(max0);
  //  }
  //  if (in < min0) {
  //    min0 = in;
  //    Serial.print("Min ");
  //    Serial.println(min0);
  //  }

//  mean = (mean * (n++) + in) / n;
//  Serial.println(mean);
//  delayMicroseconds(100);
}
