bool flag = 1, toggle = 1;
unsigned char counter=0, click = 0;
int time;
void setup()
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(3, INPUT);
  noInterrupts();
  TCCR1B = 0; //reset itmer register
  TCCR1A = 0;
  counter = 256;
  TCNT1 = counter;
  TCCR1B |= (1 << CS12); // 256 / 1 sec
  TIMSK1 |= (1 << TOIE1);
  interrupts();
}

void loop()
{
  if ((digitalRead(3)==0) && (flag == 1)){ flag = !flag; }
  if((digitalRead(3)==1 )&& (flag == 0)){ click++; flag = 1; } // change after click
  if(click>3){click = 1;}
  counter = counter*click;
  Serial.println("Mode: "+ String(click));

}

ISR (TIMER1_OVF_vect){
  TCNT1 = counter;
  digitalWrite(8, toggle);
  toggle = !toggle;
}
