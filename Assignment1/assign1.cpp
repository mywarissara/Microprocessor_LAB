#include <Wire.h>

//LiquidTWI2.h:
#include <inttypes.h>
#include "Print.h"

// for memory-constrained projects, comment out the MCP230xx that doesn't apply
#define MCP23008 // Adafruit I2C Backpack

// if DETECT_DEVICE is enabled, then when constructor's detectDevice != 0
// device will be detected in the begin() function...
// if the device isn't detected in begin() then we won't try to talk to the
// device in any of the other functions... this allows you to compile the
// code w/o an LCD installed, and not get hung in the write functions
#define DETECT_DEVICE // enable device detection code

#define MCP23008_ADDRESS 0x20

// registers
#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_BACKLIGHT 0x08 // used to pick out the backlight flag since _displaycontrol will never set itself

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
//we only support 4-bit mode #define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidTWI2 : public Print {
public:
LiquidTWI2(uint8_t i2cAddr,uint8_t detectDevice=0,uint8_t backlightInverted=0);

void begin(uint8_t cols, uint8_t rows,uint8_t charsize = LCD_5x8DOTS);

#ifdef DETECT_DEVICE
uint8_t LcdDetected() { return _deviceDetected; }
#endif // DETECT_DEVICE
void clear();
void home();

void noDisplay();
void display();
void noBlink();
void blink();
void noCursor();
void cursor();
void scrollDisplayLeft();
void scrollDisplayRight();
void leftToRight();
void rightToLeft();
void autoscroll();
void noAutoscroll();

void setBacklight(uint8_t status);

void createChar(uint8_t, uint8_t[]);
void setCursor(uint8_t, uint8_t);
#if defined(ARDUINO) && (ARDUINO >= 100) // scl
virtual size_t write(uint8_t);
#else
virtual void write(uint8_t);
#endif
void command(uint8_t);

private:
void send(uint8_t, uint8_t);
void burstBits8(uint8_t);

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines,_currline;
uint8_t _i2cAddr;
uint8_t _backlightInverted;
#ifdef DETECT_DEVICE
uint8_t _deviceDetected;
#endif // DETECT_DEVICE

};



//====================================================
//LiquidTWI2.cpp: 


/*
  LiquidTWI2 High Performance i2c LCD driver for MCP23008 & MCP23017
  hacked by Sam C. Lin / http://www.lincomatic.com
  from 
   LiquidTWI by Matt Falcon (FalconFour) / http://falconfour.com
   logic gleaned from Adafruit RGB LCD Shield library
   Panelolu2 support by Tony Lock / http://blog.think3dprint3d.com
   enhancements by Nick Sayer / https://github.com/nsayer

  Compatible with Adafruit I2C LCD backpack (MCP23008) and
  Adafruit RGB LCD Shield
*/

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <Wire.h>
#if defined(ARDUINO) && (ARDUINO >= 100) //scl
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

static inline void wiresend(uint8_t x) {
#if ARDUINO >= 100
  Wire.write((uint8_t)x);
#else
  Wire.send(x);
#endif
}

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 0; 4-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidTWI2 constructor is called). This is why we save the init commands
// for when the sketch calls begin(), except configuring the expander, which
// is required by any setup.

LiquidTWI2::LiquidTWI2(uint8_t i2cAddr,uint8_t detectDevice, uint8_t backlightInverted) {
  // if detectDevice != 0, set _deviceDetected to 2 to flag that we should
  // scan for it in begin()
#ifdef DETECT_DEVICE
  _deviceDetected = detectDevice ? 2 : 1;
#endif

  _backlightInverted = backlightInverted;

  //  if (i2cAddr > 7) i2cAddr = 7;
  _i2cAddr = i2cAddr; // transfer this function call's number into our internal class state
  _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS; // in case they forget to call begin() at least we have something
}

void LiquidTWI2::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  delay(50);

  Wire.begin();

  uint8_t result;

	  
    // now we set the GPIO expander's I/O direction to output since it's soldered to an LCD output.
    Wire.beginTransmission(MCP23008_ADDRESS | _i2cAddr);
    wiresend(MCP23008_IODIR);
    wiresend(0x00); // all output: 00000000 for pins 1...8
    result = Wire.endTransmission();
#ifdef DETECT_DEVICE
    if (result) {
        if (_deviceDetected == 2) {
          _deviceDetected = 0;
          return;
        }
    }
#endif 

#ifdef DETECT_DEVICE
  // If we haven't failed by now, then we pass
  if (_deviceDetected == 2) _deviceDetected = 1;
#endif

  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  //put the LCD into 4 bit mode
  // start with a non-standard command to make it realize we're speaking 4-bit here
  // per LCD datasheet, first command is a single 4-bit burst, 0011.
  //-----
  //  we cannot assume that the LCD panel is powered at the same time as
  //  the arduino, so we have to perform a software reset as per page 45
  //  of the HD44780 datasheet - (kch)
  //-----
    // bit pattern for the burstBits function is
    //
    //  7   6   5   4   3   2   1   0
    // LT  D7  D6  D5  D4  EN  RS  n/c
    //-----
    burstBits8(B10011100); // send LITE D4 D5 high with enable
    burstBits8(B10011000); // send LITE D4 D5 high with !enable
    burstBits8(B10011100); //
    burstBits8(B10011000); //
    burstBits8(B10011100); // repeat twice more
    burstBits8(B10011000); //
    burstBits8(B10010100); // send D4 low and LITE D5 high with enable
    burstBits8(B10010000); // send D4 low and LITE D5 high with !enable

  delay(5); // this shouldn't be necessary, but sometimes 16MHz is stupid-fast.

  command(LCD_FUNCTIONSET | _displayfunction); // then send 0010NF00 (N=lines, F=font)
  delay(5); // for safe keeping...
  command(LCD_FUNCTIONSET | _displayfunction); // ... twice.
  delay(5); // done!

  // turn on the LCD with our defaults. since these libs seem to use personal preference, I like a cursor.
  _displaycontrol = (LCD_DISPLAYON|LCD_BACKLIGHT);
  display();
  // clear it off
  clear();

  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);
}

/********** high level commands, for the user! */
void LiquidTWI2::clear()
{
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidTWI2::home()
{
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidTWI2::setCursor(uint8_t col, uint8_t row)
{
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) row = _numlines - 1;    // we count rows starting w/0
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidTWI2::noDisplay() {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidTWI2::display() {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidTWI2::noCursor() {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidTWI2::cursor() {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidTWI2::noBlink() {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidTWI2::blink() {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidTWI2::scrollDisplayLeft(void) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidTWI2::scrollDisplayRight(void) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidTWI2::leftToRight(void) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidTWI2::rightToLeft(void) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidTWI2::autoscroll(void) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidTWI2::noAutoscroll(void) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidTWI2::createChar(uint8_t location, uint8_t charmap[]) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */
inline void LiquidTWI2::command(uint8_t value) {
  send(value, LOW);
}
#if defined(ARDUINO) && (ARDUINO >= 100) //scl
inline size_t LiquidTWI2::write(uint8_t value) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return 1;
#endif
  send(value, HIGH);
  return 1;
}
#else
inline void LiquidTWI2::write(uint8_t value) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  send(value, HIGH);
}
#endif

/************ low level data pushing commands **********/

// Allows to set the backlight, if the LCD backpack is used
void LiquidTWI2::setBacklight(uint8_t status) {
#ifdef DETECT_DEVICE
  if (!_deviceDetected) return;
#endif
  if (_backlightInverted) status ^= 0x7;
    bitWrite(_displaycontrol,3,status); // flag that the backlight is enabled, for burst commands
    burstBits8((_displaycontrol & LCD_BACKLIGHT)?0x80:0x00);
}

// write either command or data, burst it to the expander over I2C.
void LiquidTWI2::send(uint8_t value, uint8_t mode) {
    // BURST SPEED, OH MY GOD
    // the (now High Speed!) I/O expander pinout
    // RS pin = 1
    // Enable pin = 2
    // Data pin 4 = 3
    // Data pin 5 = 4
    // Data pin 6 = 5
    // Data pin 7 = 6
    byte buf;
    // crunch the high 4 bits
    buf = (value & B11110000) >> 1; // isolate high 4 bits, shift over to data pins (bits 6-3: x1111xxx)
    if (mode) buf |= 3 << 1; // here we can just enable enable, since the value is immediately written to the pins
    else buf |= 2 << 1; // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
    buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
    burstBits8(buf); // bits are now present at LCD with enable active in the same write
    // no need to delay since these things take WAY, WAY longer than the time required for enable to settle (1us in LCD implementation?)
    buf &= ~(1<<2); // toggle enable low
    burstBits8(buf); // send out the same bits but with enable low now; LCD crunches these 4 bits.
    // crunch the low 4 bits
    buf = (value & B1111) << 3; // isolate low 4 bits, shift over to data pins (bits 6-3: x1111xxx)
    if (mode) buf |= 3 << 1; // here we can just enable enable, since the value is immediately written to the pins
    else buf |= 2 << 1; // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
    buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
    burstBits8(buf);
    buf &= ~( 1 << 2 ); // toggle enable low (1<<2 = 00000100; NOT = 11111011; with "and", this turns off only that one bit)
    burstBits8(buf);
}

void LiquidTWI2::burstBits8(uint8_t value) {
  // we use this to burst bits to the GPIO chip whenever we need to. avoids repetitive code.
  Wire.beginTransmission(MCP23008_ADDRESS | _i2cAddr);
  wiresend(MCP23008_GPIO);
  wiresend(value); // last bits are crunched, we're done.
  while(Wire.endTransmission());
}

//user code: ========================================================

//Address of slave device on I2C bus
LiquidTWI2 lcd(0x20);
#include <stdio.h>
#define MOTOR_D1_PIN 7
#define MOTOR_D2_PIN 8
#define MOTOR_PWM_PIN 6
#define stopbut 9
#define startbut 10
#define resetbut A1
#define cw 5
#define ccw 4
int interruptA = 2;
int interruptB = 3;
int delay_count = 0;
int getvalue = 100;
int count = 0;
bool dir = 0; // 0 cw 1 ccw
int errord = 0;
float kp = 1.0;
float ki = 0.01;
float kd = 0;
float rpm_target = 0;
float rpm_now = 0;
float pid_i=0;
int speed;
int rpm = 0;
volatile float timer1_ovf;
String buff;
String setpointStr;
unsigned long seconds0 = 0, seconds1 = 0;
bool stop = 0;
bool start = 0;
unsigned char celsius=0;
byte heart[] = {
  B00000,
  B01010,
  B10101,
  B10001,
  B10001,
  B01010,
  B00100,
  B00000
};

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("Hello guys!"); 
  lcd.setCursor(2, 1);
  lcd.print("we are TM <3");
  lcd.createChar(11, heart);
  lcd.write(0);

  pinMode(MOTOR_D1_PIN,OUTPUT); 
  pinMode(MOTOR_D2_PIN,OUTPUT); 
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(13,OUTPUT); // buzzer
  pinMode(11,OUTPUT); // green led
  pinMode(12,OUTPUT); // red led
  pinMode(A2, OUTPUT);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(interruptA, INPUT_PULLUP);
  pinMode(interruptB, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptA), 
                  a_callback, RISING);
 

  TCCR1A = 0; // initialize timer1
  TCCR1B = 0; //mode 0

  TCCR1B |= (1 << CS12);  // prescaler 256
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  timer1_ovf = 65535 - (0.01 / 0.000016);
  TCNT1 = uint16_t(timer1_ovf); // preload timer
  
  sei(); //Global Interrupt
  lcd.clear();
}

void loop(){
  getValue_function();
  checkbutton();
  temp();
  lcd_show();
  delay(100);
  
}

void lcd_show(){
  
  lcd.setCursor(0,0);
  lcd.print("encode:");
  lcd.setCursor(7,0);
  lcd.print(count);
  
  lcd.setCursor(0,1);
  lcd.print("rpm:");
  lcd.setCursor(4,1);
  lcd.print(rpm);
  
  lcd.setCursor(8,1);
  lcd.print("tmp:");
  lcd.setCursor(12,1);
  lcd.print(celsius);
  lcd.setCursor(14,1);
  lcd.print("C");
  
}
void checkbutton(){
  if (digitalRead(stopbut) == 0){ 
    digitalWrite(13,1);
    stop = 1;
  }
  else if(digitalRead(stopbut)){
    digitalWrite(13,0);
  }
  
  if (digitalRead(startbut) == 0){ 
    digitalWrite(11,1);
    stop = 0;
    delay(200);
  }
  else if(digitalRead(stopbut)){
    digitalWrite(11,0);
  }
  
  if (digitalRead(resetbut) == 0){ 
    reset();
    digitalWrite(11,1);
    digitalWrite(12,1);
    digitalWrite(A2,1);
    delay(100);
  }
  else if(digitalRead(resetbut)){
    digitalWrite(11,0);
    digitalWrite(12,0);
    digitalWrite(A2,0);
  }
}
bool toggle;
void blink(){
    if (toggle == 1){
     digitalWrite(A2,1);
     toggle = 0;}
    else if(toggle == 0){
     digitalWrite(A2,0);
     toggle = 1;}
    delay(100);
}
void reset(){
  count = 0;
  stop = 1;
}
void temp(){
  celsius = map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
  if (celsius > 50 && celsius <= 60){
    blink(); 
    digitalWrite(12,0);
    digitalWrite(13,0);
   } // blink on until temp get lower
  
  else if (celsius > 60){ 
    digitalWrite(12,1);
    digitalWrite(13,1);
}
  
  else{
    digitalWrite(A2,0);
    digitalWrite(12,0);
    digitalWrite(13,0);
  }
}
void stop_func(){
 
  digitalWrite(13,1);
  delay(100);
  digitalWrite(13,0);
}
void getValue_function(){
  char inByte = Serial.read();
    if(inByte == 's'&& buff.length() == 0){buff += inByte;}
    if(inByte == '-'&& buff.length() == 1){buff += inByte;}
    if(('0'<= inByte) && (inByte <= '9')){buff += inByte;}
    if(inByte == 'n')
    {
      if(buff[0] == 's' && buff[1] != '-')
      {
        for(int i=1;i<buff.length();i++)
        {
         if(('0'<= buff[i]) && (buff[i] <= '9')){setpointStr += buff[i];} 
        }
      }
      if(buff[0] == 's' && buff[1] == '-')
      {
        for(int i=1;i<buff.length();i++)
        {
         if(buff[i] == '-' && setpointStr == ""){setpointStr += buff[i];} 
         if(('0'<= buff[i]) && (buff[i] <= '9')){setpointStr += buff[i];}
        }
      }
      
      int temp_speed = setpointStr.toInt();
      getvalue = map(temp_speed, -100, 100, -255, 255);
      
      if(getvalue > 255){
        getvalue = 255;
      }
      if(getvalue < -255){
        getvalue = -255;
      }
      buff = "";
      setpointStr = "";
      Serial.println(getvalue);
      
  }
}

void a_callback(){
  if (!digitalRead(interruptB)){count++;}
  else if(digitalRead(interruptB)){count--;}
}

void moveForward(int speed) {
    digitalWrite(MOTOR_D1_PIN,HIGH);
    digitalWrite(MOTOR_D2_PIN,LOW); 
    analogWrite(MOTOR_PWM_PIN,speed);
}

void moveBackward(int speed) {
    digitalWrite(MOTOR_D1_PIN,LOW);
    digitalWrite(MOTOR_D2_PIN,HIGH); 
    analogWrite(MOTOR_PWM_PIN,speed);
}
void setSpeed(int speed) { 
  if (speed > 0) {
    if (speed > 255) {
    	speed = 255;
    }
  	moveForward(speed);
  } else if (speed < 0) {  
  	speed = speed*(-1);
    
    if (speed > 255) {
    	speed = 255;
    }
    moveBackward(speed);
  }
  else {
    moveForward(1);
  }
}

ISR(TIMER1_OVF_vect)
{
  
  bool cw_read = digitalRead(A4); // cw dir = 0
  bool ccw_read = digitalRead(A5); // ccw dir =1
   
  if (!cw_read){getvalue = -1*abs(getvalue);}
  if (!ccw_read){getvalue = abs(getvalue);}
  //seconds1 = millis();
  rpm = (count*6000/226*(0.01));
  
  float error_now = getvalue - rpm; //  error
  pid_i += error_now*0.01;
  float pid_d = (error_now - errord)/0.01;
  int pid = (kp*error_now) + (ki*pid_i) + (kd*pid_d);
  
  if (stop == 1){pid = 1;}
           
  
  setSpeed(pid); 
  
  //seconds0 = seconds1;
  errord = error_now;
  
  count = 0;
  //Serial.print(getvalue); 
  //Serial.print(",");
  //Serial.println(pid);

  TCNT1 = uint16_t(timer1_ovf);
}

