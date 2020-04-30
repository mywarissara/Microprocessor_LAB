#include<SPI.h>

#define DATA 11
#define CLK 13
#define SS 10
#define SS_2 9
#define btnIn 2
#define btnDe 3
#define btnCount 4
#define buzzer 5
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
unsigned char count = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SS, OUTPUT);
  pinMode(SS_2, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  pinMode(A0,INPUT_PULLUP);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalRead(8) == 0? photo():timeinterval(); 
  // mode0 = auto use photosensor to turn on light, 
  // mode1= manual use button to set timer
  
}

unsigned char plus = 0, minus = 0,ok = 0, count_tmp = 0;
bool flagp =0, flagm =0, flagok =0;

void timeinterval(){
  plus = digitalRead(2);
  minus = digitalRead(3);
  ok = digitalRead(4);
  
  if (plus == 0&&flagp==0){count_tmp++;
                spi_1(num[count_tmp]);
                flagp=1;
                Serial.println(count_tmp);}
  else if (plus == 1&&flagp ==1){flagp=0;}
  if (minus == 0&& flagm ==0){
                 count_tmp--;
                 spi_1(num[count_tmp]);
                 flagm = 1;
                }
  else if (minus == 1&& flagm==1){flagm =0;}
  if (count_tmp < 0){count_tmp = 0;}
  if (count_tmp >9){count_tmp = 9;}

  if (ok==0&& flagok ==0){count = count_tmp;
             spi_1(num[count_tmp]);
             flagok = 1;
             control_bulb();
          }
  else if(ok ==1 && flagok ==1){flagok =0;
                               count_tmp = 0;}
 
}
void photo(){
  Serial.println(analogRead(A0));
  if(analogRead(A0)<15){
     digitalWrite(5,1);
      digitalWrite(6,1);
    digitalWrite(7,1);
  }
  else if(analogRead(A0)<25){
     digitalWrite(5,1);
      digitalWrite(6,1);
    digitalWrite(7,0);
  }
  else if (analogRead(A0)<45){
     digitalWrite(5,1);
    digitalWrite(6,0);
     digitalWrite(7,0);}
 
  if (analogRead(A0)>=60){
     digitalWrite(5,0);
     digitalWrite(6,0);
     digitalWrite(7,0);}
}

void control_bulb(){
  int interv = count_tmp / 3;
  
  for(int i = 5; i<8;i++){
    digitalWrite(i,1);
    delay(interv*1000);
    change_number();
  }
  Serial.println(interv*1000);
  
  if (count == 0){  digitalWrite(5,0);
                 digitalWrite(6,0);
                 digitalWrite(7,0);}
}

void spi_1(unsigned char cData){
  shiftOut(DATA, CLK, MSBFIRST, cData);
  digitalWrite(SS, 1);
  digitalWrite(SS, 0);
  digitalWrite(SS_2, 0);
}

void change_number(){
  if (count < 0 ){count=0;}
  if (count>0){count--;
              spi_1(num[count]);
              }
  delay(1000);
}

