#include <stdio.h>
#define MOTOR_D1_PIN 7
#define MOTOR_D2_PIN 8
#define MOTOR_PWM_PIN 6

int interruptA = 2;
int interruptB = 3;
int delay_count = 0;
int getvalue = -100;
int count = 0;
bool dir = 0; // 0 cw 1 ccw
int errord = 0;
float kp = 1;
float ki = 0.01;
float kd = 3;
float rpm_target = 0;
float rpm_now = 0;
float pid_i=0;
int speed;
String buff, inString;
unsigned long rpm = 0;
volatile float timer1_ovf;

unsigned long seconds0 = 0, seconds1 = 0;
void setup()
{
  Serial.begin(9600);
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
  TCCR1A = 0; // initialize timer1
  TCCR1B = 0; //mode 0

  TCCR1B |= (1 << CS12);  // prescaler 256
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  timer1_ovf = 65535 - (0.01 / 0.000016);
  TCNT1 = uint16_t(timer1_ovf); // preload timer
  
  sei(); //Global Interrupt
}

void loop(){
  getValue_function();
  //delay(100);
  
}
void getValue_function(){
  while (Serial.available()){
      char inByte = Serial.read();
    if(inByte == 's'&& buff.length() == 0){buff += inByte;}
    if(inByte == '-'&& buff.length() == 1){buff += inByte;}
    if(('0'<= inByte) && (inByte <= '9')){buff += inByte;}
    if(inByte == 'n')
    {
      if(buff[0] == 's' && buff[1] != '-')
      {
        for(int i=1;i<buff.length();i++)
        {
         if(('0'<= buff[i]) && (buff[i] <= '9')){inString += buff[i];} 
        }
      }
      if(buff[0] == 's' && buff[1] == '-')
      {
        for(int i=1;i<buff.length();i++)
        {
         if(buff[i] == '-' && inString == ""){inString += buff[i];} 
         if(('0'<= buff[i]) && (buff[i] <= '9')){inString += buff[i];}
        }
      }
    }
    getvalue = inString.toInt();
       // Serial.println(getvalue);
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

ISR(TIMER1_OVF_vect)
{
  //seconds1 = millis();
  rpm = (count*6000/226*(0.01));
  
  float error_now = abs(getvalue) - rpm; //  error
  pid_i += error_now*0.01;
  float pid_d = (error_now - errord)/0.01;
  int pid = (kp*error_now) + (ki*pid_i) + (kd*pid_d);
  
  getvalue < 0? pid = pid : pid = -pid;
  setSpeed(-pid); 
  //seconds0 = seconds1;
  errord = error_now;
  
  count = 0;
  Serial.print(getvalue);
  //Serial.println(rpm);
  //Serial.println(error_now);
  //Serial.println(pid_i);
  //Serial.println(errord);
  //Serial.println(pid_d);
  Serial.print(",");
  Serial.println(pid);
  //Serial.println("*******");

  TCNT1 = uint16_t(timer1_ovf);
}