bool flag = 1;
void setup()
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), ISR_Callback, FALLING);
}

//rising 0v to 5V, falling 5v to 0v, change 0v to 5v or 5v to 0v
void loop()
{
  digitalWrite(9, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(9, 0);
  delay(1000); // Wait for 1000 millisecond(s)
  Serial.println(digitalRead(3));
}

void ISR_Callback(){
  digitalWrite(8, flag);
  flag = !flag;
}