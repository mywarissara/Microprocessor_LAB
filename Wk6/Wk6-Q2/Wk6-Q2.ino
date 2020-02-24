/*
 * The time from program and time from timer clock is almost the same 
 * difference of it is only 0.14 sec 
 * I think it happens because of the error between the program
 * but it is very little so it might not be recognize 
 * 
 * warissara 61010946
 * 
 */#include <ThreeWire.h>  
#include <RtcDS1302.h>
#define led 6
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire>Rtc(myWire);
int sectime =0, sectime_begin;
void setup() {
  Serial.begin(9600);
  Rtc.Begin();
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
