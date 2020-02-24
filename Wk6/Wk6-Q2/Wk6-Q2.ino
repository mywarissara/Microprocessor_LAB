#include <ThreeWire.h>  
#include <RtcDS1302.h>
#define led 6
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire>Rtc(myWire);
int sectime =0, sectime_begin;
//void printDateTime(const RtcDateTime& dt){
//  Serial.print(dt.Year(), DEC);
//  Serial.print('/');
//  Serial.print(dt.Month(), DEC);
//  Serial.print('/');
//  Serial.print(dt.Day(), DEC);
//  Serial.print(' ');
//  Serial.print(dt.Hour(), DEC);
//  Serial.print(':');
//  Serial.print(dt.Minute(), DEC);
//  Serial.print(':');
//  Serial.print(dt.Second(), DEC);
//  Serial.println();
//  Serial.print("Total Seconds since 1/1/2000 : ");
//  Serial.print(dt.TotalSeconds());
//  Serial.println();
//  }
void setup() {
  Serial.begin(9600);
  Rtc.Begin();
  RtcDateTime setting_time(2020,2,2,23,31,10);// year, month, dayOfMonth, hour, minute, second
  Rtc.SetDateTime(setting_time);
  Serial.println("Start");
  pinMode(led, OUTPUT);
  RtcDateTime now = Rtc.GetDateTime();
  sectime_begin = (now.Hour() * 60 * 100) + (now.Minute() * 60 ) +(now.Second());
 
  }
  void loop() {
    toggle();
    delay(1000);
}
void toggle(){
  RtcDateTime now = Rtc.GetDateTime();
  sectime = (now.Hour() * 60 * 100) + (now.Minute() * 60 ) +(now.Second());
  
  if (sectime <= (sectime_begin + (2*60))){
    sectime % 2 == 0 ? digitalWrite(led,1): digitalWrite(led,0);
    Serial.println(now.Second());
  }
  else{
    Serial.println("Stop");
    digitalWrite(led,0);
  }
}
