#define MOTOR_D1_PIN 7 
#define MOTOR_D2_PIN 8 
#define MOTOR_PWM_PIN 6
int interruptChannelAPin = 2;
int interruptChannelBPin = 3;
int delay_count = 0;
int speed = 1000;

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
  Serial.begin(9600);
  setSpeed(1);
}

void loop()
{
  if(Serial.available()){
    String inString = Serial.readString();
    speed = inString.toInt();
   	Serial.println(speed);
  }
  setSpeed(speed); // pid = -255 --> 255

}
