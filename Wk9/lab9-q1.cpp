bool toggle = 1;
unsigned char click = 0;
int time;
void setup()
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), ISR_Callback, RISING);
}

//rising 0v to 5V, falling 5v to 0v, change 0v to 5v or 5v to 0v
void loop()
{
  if(click>3){click = 1;}
  time = click*1000;
  Serial.println("Mode: "+ String(click));
   digitalWrite(8, toggle);
  toggle = !toggle;
   delay(time);
}

void ISR_Callback(){
  click++;
}
