#include <SPI.h>
#include <Wire.h>
#include <ThreeWire.h> 
#include <RtcDS1302.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
ThreeWire myWire(8,9,10); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
void setup(){
  Serial.begin(9600);
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Serial.println();

   if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
    
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  beginDisplay();
  
}
void loop(){
  display.clearDisplay();
//  onSW();
  RTC();
  
}
void onSW(){
  if (digitalRead(2) == 0){
  display.drawCircle(SCREEN_WIDTH/4, SCREEN_HEIGHT/2, 20, WHITE);
  display.fillCircle(SCREEN_WIDTH - SCREEN_WIDTH/4 , SCREEN_HEIGHT/2, 20, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH/4 - 7,SCREEN_HEIGHT/2 -2);
  display.println("SW1");
  display.setTextColor(BLACK);
  display.setCursor((SCREEN_WIDTH - SCREEN_WIDTH/4) - 7, SCREEN_HEIGHT/2 -2);
  display.println("SW2");
  display.display();
  }
  
  if (digitalRead(3) == 0){
  display.fillCircle(SCREEN_WIDTH/4, SCREEN_HEIGHT/2, 20, WHITE);
  display.drawCircle(SCREEN_WIDTH - SCREEN_WIDTH/4 , SCREEN_HEIGHT/2, 20, WHITE);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(SCREEN_WIDTH/4 - 7,SCREEN_HEIGHT/2 -2);
  display.println("SW1");
  display.setTextColor(WHITE);
  display.setCursor((SCREEN_WIDTH - SCREEN_WIDTH/4) - 7, SCREEN_HEIGHT/2 -2);
  display.println("SW2");
  display.display();
  }
  
  if (digitalRead(3) ==1 && digitalRead(2) ==1 ){
  beginDisplay();
  }
  if(digitalRead(3) ==0 && digitalRead(2) ==0 ){
  display.drawCircle(SCREEN_WIDTH/4, SCREEN_HEIGHT/2, 20, WHITE);
  display.drawCircle(SCREEN_WIDTH - SCREEN_WIDTH/4 , SCREEN_HEIGHT/2, 20, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH/4 - 7,SCREEN_HEIGHT/2 -2);
  display.println("SW1");
  display.setTextColor(WHITE);
  display.setCursor((SCREEN_WIDTH - SCREEN_WIDTH/4) - 7, SCREEN_HEIGHT/2 -2);
  display.println("SW2");
  display.display();
  }
}
void beginDisplay(){
  
  display.clearDisplay(); // clears the screen and buffer
  display.fillCircle(SCREEN_WIDTH/4, SCREEN_HEIGHT/2, 20, WHITE);
  display.fillCircle(SCREEN_WIDTH - SCREEN_WIDTH/4 , SCREEN_HEIGHT/2, 20, WHITE);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(SCREEN_WIDTH/4 - 7,SCREEN_HEIGHT/2 -2);
  display.println("SW1");
  display.setCursor((SCREEN_WIDTH - SCREEN_WIDTH/4) - 7, SCREEN_HEIGHT/2 -2);
  display.println("SW2");
  display.display();

}

void RTC(){
  RtcDateTime now = Rtc.GetDateTime();
  display.clearDisplay();
  Serial.println(now.Year());
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH/3 - 7,SCREEN_HEIGHT/2 -2);
  Serial.println(String(now.Year()) +" / "+ String(now.Month()) +" / "+ String(now.Day() ));
  display.setCursor(SCREEN_WIDTH/3 ,SCREEN_HEIGHT/2 + 10);
  Serial.println(String(now.Hour() )+ " : "+ String( now.Minute()) + " : " + String(now.Second()));
//  display.setCursor((SCREEN_WIDTH - SCREEN_WIDTH/4) - 7, SCREEN_HEIGHT/2 -2);
//  display.print(" ");
//  display.print();
  display.display();
}
