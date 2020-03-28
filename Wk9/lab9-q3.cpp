#include <SPI.h>

#define DATA 11
#define CLK 13
#define SS 10
#define SS_2 9
#define a  0b00000001
#define b  0b00000010
#define c  0b00000100
#define d  0b00001000
#define e  0b00010000
#define f  0b00100000
#define g  0b01000000
#define dp 0b10000000

#define num0 a | b | c | d | e | f
#define num1 b | c
#define num2 a | b | d | e | g
#define num3 a | b | c | d | g
#define num4 b | c | f | g
#define num5 a | c | d | f | g
#define num6 a | c | d | e | f | g
#define num7 a | b | c | f
#define num8 a | b | c | d | e | f | g 
#define num9 a | b | c | d | f | g

unsigned char num[10] = {num0, num1, num2, num3, num4, num5, num6, num7, num8, num9};
unsigned char counter=0;
int count = -1;

void spi_1(unsigned char cData){
  shiftOut(DATA, CLK, MSBFIRST, cData);
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  digitalWrite(SS_2, HIGH);
  digitalWrite(SS_2, LOW);
}

void setup(){
  pinMode(SS_2, OUTPUT);
  pinMode(SS, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);
  Serial.begin(9600);
  noInterrupts();
  TCCR1B = 0; //reset itmer register
  TCCR1A = 0;
  counter = 256;
  TCNT1 = counter;
  TCCR1B |= (1 << CS12); // 256/sec
  TIMSK1 |= (1 << TOIE1);
  interrupts();
}

void loop()
{
  Serial.println("Num: "+ String(count));
}

ISR (TIMER1_OVF_vect){
  TCNT1 = counter;
  count++;
  if(count>9){ count = 0; }
  spi_1(num[count]);
}
