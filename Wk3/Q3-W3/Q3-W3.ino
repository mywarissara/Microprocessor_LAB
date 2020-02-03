int clk_pin =2;
int latch_pin = 3;
int data_pin = 4;
int buttonR = 5;
int buttonL = 6;
int number[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
int out = 0, num = 0, R, L, right = 0, left = 0;
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
  
}

void latch(int shift){
    digitalWrite(latch_pin,LOW);
    for (int i = 0 ; i < 8 ; i++){
      digitalWrite(data_pin,shift >> (7-i)& 0x01);
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
   Serial.println(right);
   Serial.println(left);
  latch(number[num]);
  delay(500);
}


void moveLeft(){
  num <= 0 ? num = 9 : num--;
}
void moveRight(){
  num >=9 ? num = 0 : num++;
}
