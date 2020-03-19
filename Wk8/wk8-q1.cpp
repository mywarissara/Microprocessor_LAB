String buff;
int led, brightness;
bool flag = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop()
{
  if (Serial.available()>0){
   char inByte = Serial.read();
    buff += inByte;
    if (buff == "L1"){
      buff = "";
      led = 10;
    }
    else if (buff == "L2"){
      buff = "";
       led = 11;
    }
    if (buff == "B"){
      buff = "";
    }
    if(inByte == 'B'){
      brightness = buff.toInt();
      Serial.println(brightness);
      analogWrite(led ,int(brightness*2.54));
    }
    delay(100);
  }
}