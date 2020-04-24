#include <stdio.h>
#define MOTOR_D1_PIN 7
#define MOTOR_D2_PIN 8
#define MOTOR_PWM_PIN 9

int interruptA = 2;
int interruptB = 3;
int delay_count = 0;
int getvalue = 0;
int count = 0;
bool dir = 0; // 0 cw 1 ccw
unsigned long seconds0 = 0, seconds1 = 0;
void setup()
{
  pinMode(MOTOR_D1_PIN,OUTPUT); 
  pinMode(MOTOR_D2_PIN,OUTPUT); 
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(A0, INPUT);
  
  pinMode(interruptA, INPUT_PULLUP);
  pinMode(interruptB, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptA), 
                  a_callback, RISING);
  
  attachInterrupt(digitalPinToInterrupt(interruptB), 
                  b_callback, RISING);
  Serial.begin(9600);
}
int errord = 0;
float kp = 0.3;
float ki = 0.07;
float kd = 0.1;
float rpm_target = 0;
float rpm_now = 0;
int speed;
void loop()
{
  seconds1 = millis();
  float error_now = getvalue - count; // position control
  float pid_i = error_now*(seconds1 - seconds0)/1000;
  float pid_d = (error_now - errord)/(seconds1 - seconds0)/1000;
  int pid = (kp*error_now) + (ki*pid_i) + (kd*pid_d);
  setSpeed(pid); // pid = -255 --> 255

  getValue_function();
  //rpm_target = getvalue/60; // speed control
  //rpm_now = (count/100)/(seconds1/1000/60);
  //float error_now = rpm_target - rpm_now; // speed control
  //float pid_i = error_now*(seconds1 - seconds0)/1000;
  //float pid_d = (error_now - errord)/(seconds1 - seconds0)/1000;
  //int pid = (kp*error_now) + (ki*pid_i) + (kd*pid_d);
  //setSpeed(pid);
  
  Serial.print(getvalue);
  Serial.print(",");
  Serial.println(count);
  //show_on_7segment(count); // show on 7 segment
  seconds0 = seconds1;
  errord = error_now;
  delay(100);
  
}
void getValue_function(){
  if(Serial.available()){
        String inString = Serial.readString();
        getvalue = inString.toInt();
        Serial.println(getvalue);
  }
  //else{
  //  getvalue = analogRead(A0);
  //}
}
void a_callback(){
    if (digitalRead(interruptA)==1 && digitalRead(interruptB) == 0){
        count++;
        digitalWrite(6,0); // turn off led
    }
}
void b_callback(){
    if (digitalRead(interruptA)==0 && digitalRead(interruptB) == 1){
        count--;
        digitalWrite(6,1); // led turn on
    }
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