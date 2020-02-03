int clk_pin =2;
int latch_pin = 3;
int data_pin = 4;
int number[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
int out = 0, num = 5;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(clk_pin,OUTPUT);
pinMode(latch_pin,OUTPUT);
pinMode(data_pin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
  num = Serial.read();
  Serial.println(num);
  latch(number[num-48]);
  delay(500);
  }
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
