#define CAPTURE_AND_SEND_DOT 67

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(2, INPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (digitalRead(2) == LOW)
  {

    Serial.write(CAPTURE_AND_SEND_DOT);
    while (Serial.available() == 0)
    {
      delay(250);
    }
    int amountDot = Serial.read();
    Serial.write(amountDot);
    //    Serial.print("AmountDot = ");
    //    Serial.println(amountDot);
  }
}
