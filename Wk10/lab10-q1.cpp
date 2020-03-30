#define MOTOR_D1_PIN 7 //direction
#define MOTOR_D2_PIN 8 //direction
#define MOTOR_PWM_PIN 9
bool previous_read;
int count = 0;

void setup() {
  pinMode(MOTOR_D1_PIN,OUTPUT);
  pinMode(MOTOR_D2_PIN,OUTPUT);
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("TestEncoder");
  
}
void loop() {
  bool current_read = digitalRead(2);
  if((previous_read == 0) && (current_read == 1)){
    count++;
    Serial.println(count);
  }
  previous_read = current_read;
  speed(100);
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