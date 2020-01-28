int clk_pin = 13;//SHCP
int latch_pin = 10;//STCP
int data_pin = 11;//DS
int out=0;
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
pinMode(clk_pin,OUTPUT);
pinMode(latch_pin,OUTPUT);
pinMode(data_pin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  blinkLED();
  delay(500);
}

void latchData(){
  digitalWrite(latch_pin,LOW);
  for(int i = 0 ; i < 16 ; i++){
     Serial.println(out);
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
//delay(1000);
}

void blinkLED(){
  out < 15 ? out++ : out = 0;
  latchData();
}
