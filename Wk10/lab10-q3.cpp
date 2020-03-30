#define MOTOR_D1_PIN 7 //direction
#define MOTOR_D2_PIN 8 //direction
#define MOTOR_PWM_PIN 9
int channelA=0, channelB=0,count=0;
bool dir;
void setup() {
  pinMode(MOTOR_D1_PIN,OUTPUT);
  pinMode(MOTOR_D2_PIN,OUTPUT);
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("TestEncoder");
  attachInterrupt(digitalPinToInterrupt(2), count_encoderA, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), count_encoderB, FALLING);

}
void loop() {
   speed(-200); //-255 to 255
}

void count_encoderA(){
   channelA++;
}
void count_encoderB(){
   channelB++;
   check();
}

void check(){
  channelA < channelB? dir = 0: dir =1;
  Serial.print("count: "+ String(count)+ " dir: ") ;
  if (dir == 1){
    Serial.println("cw");
    count--;
  }
  else{
    Serial.println("ccw"); 
    count++;
  }
}

void speed(int _speed){
  if(_speed > 0){
  digitalWrite(MOTOR_D1_PIN,LOW);
  digitalWrite(MOTOR_D2_PIN,HIGH);
  analogWrite(MOTOR_PWM_PIN, _speed);
  }
  else{
  digitalWrite(MOTOR_D1_PIN,1);
  digitalWrite(MOTOR_D2_PIN,0);
  analogWrite(MOTOR_PWM_PIN, _speed);
  }
}