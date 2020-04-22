#define MOTOR_D1_PIN 7 
#define MOTOR_D2_PIN 8 
#define MOTOR_PWM_PIN 6
int interruptChannelAPin = 2;
int interruptChannelBPin = 3;
int delay_count = 0;
int setpoint = 1000;

int toggle=1;
//volatile int encoderACount = 0;
//volatile int encoderBCount = 0;
volatile int encoderCount = 0;
volatile int encoderDir = 0;

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
void setSpeed(int speed) { // Motor forward
  if (speed > 0) {
    if (speed > 255) {
    	speed = 255;
    }
  	moveForward(speed);
  } else if (speed < 0) {  // Motor backward
  	speed = speed*(-1);
    
    if (speed > 255) {
    	speed = 255;
    }
    moveBackward(speed);
  }
  else {                   // Motor stop
    moveForward(1);
  }
}
void setup()
{
  pinMode(MOTOR_D1_PIN,OUTPUT); 
  pinMode(MOTOR_D2_PIN,OUTPUT); 
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(A0, INPUT);
  
  pinMode(interruptChannelAPin, INPUT_PULLUP);
  pinMode(interruptChannelBPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptChannelAPin), 
                  ChannelA_callback, RISING);
  
  attachInterrupt(digitalPinToInterrupt(interruptChannelBPin), 
                  ChannelB_callback, RISING);
  Serial.begin(9600);
}


int errori = 0;
float kp = 1.0;
float ki = 0.1;
void loop()
{
  //int setpoint = analogRead(A0);
  int error = setpoint - encoderCount;
  int pid = kp*(float)(error) + ki*(float)(errori);
  errori+=error;
  setSpeed(pid); // pid = -255 --> 255
  
   
  Serial.print(setpoint);
  Serial.print(",");
  Serial.println(encoderCount);
  delay(100);
  
  if(Serial.available()){
    String inString = Serial.readString();
    setpoint = inString.toInt();
   	Serial.println(setpoint);
  }

}

void ChannelA_callback() {
  if (digitalRead(interruptChannelAPin)==1 && 
      digitalRead(interruptChannelBPin)==0) {
  	encoderCount--;
    //encoderDir = 1;
  }
}

void ChannelB_callback() {
  if (digitalRead(interruptChannelAPin)==0 && 
      digitalRead(interruptChannelBPin)==1) {
  	encoderCount++;
    //encoderDir = 0;
  }
}


