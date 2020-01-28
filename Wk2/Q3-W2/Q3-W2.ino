int clk_pin = 13;//SHCP
int latch_pin = 10;//STCP
int data_pin = 11;//DS
int VR = A0;
int buttonR = 2;
int buttonL = 3;
int out=0, R, L,right = 1, left = 0, timer;
bool clicker = true;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
pinMode(clk_pin,OUTPUT);
pinMode(latch_pin,OUTPUT);
pinMode(data_pin,OUTPUT);
pinMode(buttonR,INPUT_PULLUP);
pinMode(buttonL,INPUT_PULLUP);
pinMode(VR,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  blinkLED();
  delay(speeder());
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
  if (R == 0 && clicker == true){ // right
    left = 0;
    clicker = false;
    right++;
  }
  else if(L==0 && clicker == true){ // left
    right = 0;
    clicker = false;
    left++;
  }
  if (R == 1 && L == 1){ 
    clicker = true;
  }
  if (R == 0 && L ==0){ // stop
    right = 0;
    left = 0;
  }

  if  (right >= 1) { moveLeft();}
  if  (left >= 1) { moveRight();}
  latchData();
  Serial.println(out);
}


void moveLeft(){
  out <= 0 ? out = 15 : out--;
}
void moveRight(){
  out >=15 ? out = 0 : out++;
}

int speeder(){
  timer = analogRead(VR);
  return timer;
}
