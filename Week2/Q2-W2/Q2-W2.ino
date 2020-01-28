int clk_pin = 13;//SHCP
int latch_pin = 10;//STCP
int data_pin = 11;//DS
int buttonR = 2;
int buttonL = 3;
int out=0, R, L;
bool clicker = true;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
pinMode(clk_pin,OUTPUT);
pinMode(latch_pin,OUTPUT);
pinMode(data_pin,OUTPUT);
pinMode(buttonR,INPUT_PULLUP);
pinMode(buttonL,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  blinkLED();
  delay(100);
}

void latchData(){
  digitalWrite(latch_pin,LOW);
  for(int i = 0 ; i < 16 ; i++){
  
  if ( i == out ){
  digitalWrite(data_pin,HIGH);
  }
  else{
   digitalWrite(data_pin,LOW);
  }
  digitalWrite(clk_pin,HIGH);
  digitalWrite(clk_pin,LOW);
}
  digitalWrite(latch_pin,HIGH);
}

void blinkLED(){
  R = digitalRead(buttonR);
  L = digitalRead(buttonL);
  if (R == 0 && clicker == true){
    clicker = false;
    moveRight();
  }
  else if(L==0 && clicker == true){
    clicker = false;
    moveLeft();
  }
  if (R == 1 && L == 1){
    clicker = true;
  }
  latchData();
  Serial.println(out);
}

void moveLeft(){
  out <= 0 ? out = 15 : out--;
}
void moveRight(){
  out >=15 ? out = 0 : out++;
}
