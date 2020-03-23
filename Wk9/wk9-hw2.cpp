bool flag = 1;
unsigned char counter=0;
void setup()
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(3, INPUT);
  noInterrupts();
  TCCR1B = 0; //reset itmer register
  TCCR1A = 0;
  counter = 256;
  TCNT1 = counter;
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << TOIE1);
  interrupts();
 
}

void loop()
{
  digitalWrite(9, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(9, 0);
  delay(1000); // Wait for 1000 millisecond(s)
  Serial.println(digitalRead(3));
}

ISR (TIMER1_OVF_vect){
  TCNT1 = counter;
  digitalWrite(8, flag);
  flag = !flag;
}