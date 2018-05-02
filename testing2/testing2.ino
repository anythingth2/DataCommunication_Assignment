
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW );
}
boolean b = false;
int val = 0;
int countByte = 0;
void loop() {
  if (Serial.available() > 0) {
    int in = Serial.read();
    if (++countByte > 2)
    {
      countByte = 0;
      val = 0;
    }
    val = val << 8 + in;
  }
  digitalWrite(13, b);
}
