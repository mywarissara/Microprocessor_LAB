#include <stdio.h>
#include <LiquidCrystal.h>
#define MOTOR_D1_PIN 7
#define MOTOR_D2_PIN 8
#define MOTOR_PWM_PIN 6
#define tmp A0
#define buzzer 13
#define green A3
int interruptA = 2;
int interruptB = 3;
int delay_count = 0;
int getvalue = 1;
int count = 0;
bool dir = 0; // 0 cw 1 ccw
int errord = 0;
float kp = 1;
float ki = 0;
float kd = 0;
float rpm_target = 0;
float rpm_now = 0;
float pid_i=0;
int speed;
int rpm = 0;
volatile float timer1_ovf;
LiquidCrystal lcd(12, 11, 10, 9, 5, 4);
                 
void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  pinMode(MOTOR_D1_PIN,OUTPUT); 
  pinMode(MOTOR_D2_PIN,OUTPUT); 
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(13,OUTPUT); // buzzer
  pinMode(A3, OUTPUT);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A1, OUTPUT);
  
  pinMode(tmp, INPUT);
  pinMode(interruptA, INPUT_PULLUP);
  pinMode(interruptB, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptA), 
                  a_callback, RISING);
 

  TCCR1A = 0; // initialize timer1
  TCCR1B = 0; //mode 0

  TCCR1B |= (1 << CS12);  // prescaler 256
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  timer1_ovf = 65535 - (0.01 / 0.000016);
  TCNT1 = uint16_t(timer1_ovf); // preload timer
  
  sei(); //Global Interrupt
}
bool click=0;
void loop(){
  button();
  delay(100);
  
}
int page=0;
bool flag =0,state=0;
int tmp_m=0, photo =0, pid=0;

void lcdshow(){
     lcd.clear();
  	 lcd.setCursor(0,0);
 	 lcd.print("speed fan: ");
 	 lcd.print(pid);
     lcd.setCursor(0,1);
     lcd.print("state: ");
  if (state==0){
    lcd.print("Normal");}
  else if(state==1){
    lcd.print("Over heat");}
     delay(300);
     lcd.clear();
 	 lcd.setCursor(0,0);
     lcd.print("t: ");
     lcd.print(String(tmp_m)); 
     lcd.print(" Light: ");
     lcd.print(String(photo));
  	 lcd.setCursor(0,1);
     lcd.print("Light: ");
  if (photo>30){lcd.print("ON");}
  else{lcd.print("OFF");}
   
}
bool stop=0;
void button(){
  Serial.println(digitalRead(A4));
  if (digitalRead(A4)==0 && click ==0 && stop==0){ //emergency stop
    digitalWrite(buzzer,1);
    getvalue=1;
    digitalWrite(A1,0);
    digitalWrite(A3,0);
    click=1;
    stop=1;
  }
  else if (digitalRead(A4)==1 && click ==1 && stop==1){
  	click =0;
  }
  else if(digitalRead(A4)==1 && click ==0 && stop ==0){
    control();
  }
  if (digitalRead(A5)==0 && stop ==1){stop=0;}
}

void photosen(){
  photo = analogRead(A2);
  if ( photo>30){
    digitalWrite(A1,1);
  }
  else{ digitalWrite(A1,0);}
  lcdshow();
}
void control(){
  tmp_m = analogRead(tmp);
  
  float voltage = (tmp_m * 5.0)/1024;//(5*temp)/1024 is to convert the 10 bit number to a voltage reading.
  float milliVolt = voltage * 1000;//This is multiplied by 1000 to convert it to millivolt.
  float tmpCel =  (milliVolt-500)/10 ;//For TMP36 sensor. Range(−40°C to +125°C)
  
  if(tmpCel>100){
    digitalWrite(buzzer,1);
    state=1;
    digitalWrite(green,0);
    getvalue = 255; // fan increase
  }
  else if(tmpCel > 70){
    getvalue = 100;
    analogWrite(green,50);
    state =1;
  }
  
  else if (tmpCel > 50){
   digitalWrite(buzzer,0);
   analogWrite(green,100);
    getvalue = 50;
    state=0;
  }
  
  else if (tmpCel <= 50){
    state=0;
    digitalWrite(buzzer,0);
    digitalWrite(green,1);
    getvalue = 1;
  }
  
  photosen();
}


void a_callback(){
  if (!digitalRead(interruptB)){count++;}
  else if(digitalRead(interruptB)){count--;}
}

void moveForward(int speed) {
    digitalWrite(MOTOR_D1_PIN,HIGH);
    digitalWrite(MOTOR_D2_PIN,LOW); 
    analogWrite(MOTOR_PWM_PIN,speed);
}

void moveBackward(int speed) {
    digitalWrite(MOTOR_D1_PIN,LOW);
    digitalWrite(MOTOR_D2_PIN,HIGH); 
    analogWrite(MOTOR_PWM_PIN,speed);
}
void setSpeed(int speed) { 
  if (speed > 0) {
    if (speed > 255) {
    	speed = 255;
    }
  	moveForward(speed);
  } else if (speed < 0) {  
  	speed = speed*(-1);
    
    if (speed > 255) {
    	speed = 255;
    }
    moveBackward(speed);
  }
  else {
    moveForward(1);
  }
}

ISR(TIMER1_OVF_vect)
{
  
  rpm = (count*6000/226*(0.01));
  
  float error_now = getvalue - rpm; //  error
  pid_i += error_now*0.01;
  float pid_d = (error_now - errord)/0.01;
  pid = (kp*error_now) + (ki*pid_i) + (kd*pid_d);
  if(pid > 255)
  {
    pid = 255;
  }
    if(pid < -255)
  {
    pid = -255;
  }
  setSpeed(pid); 
  errord = error_now;
  
  count = 0;
  //Serial.print(getvalue); 
  //Serial.print(",");
  //Serial.println(pid);

  TCNT1 = uint16_t(timer1_ovf);
}