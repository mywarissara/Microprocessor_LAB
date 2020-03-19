#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial =  SoftwareSerial(13, 12);
String buff="";

void setup()
{
  Serial.begin(9600);
  mySerial.begin(4800);
  
}

void loop()
{
  if (mySerial.available()>0){
   char inByte = mySerial.read();
    if (inByte != '\n'){
      buff += inByte;
    }
    else{
    int brightness = buff.toInt();
      brightness = int(brightness * 255 / 1023);
      analogWrite(5,brightness);
      buff = "";
 	 }
     delay(100);
  }
}

