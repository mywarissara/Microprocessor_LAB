int count = 0;
#define R 0
#define L 1 
#define VR 2 

void setup()
{
  Serial.begin(9600);
  DDRD = 0xFF;
  DDRB = 0xFF;
  pinMode(R, INPUT);
  pinMode(L, INPUT);
  pinMode(VR, INPUT);
}

void ledON(){
  if ( count < 7 ){
    PORTB = 0b00000000;
    PORTD = 0b00000010 << count; 
  }
  else if ( count < 9 ){
    PORTB = 0b00000001 << (count-7);
    PORTD = 0b00000000; 
  }
  delay(analogRead(VR));
}


void loop(){
  
   if (analogRead(L) <=600){
    count = count + 1;
   }     
   else if (analogRead(R) <=600){
    count = count - 1;
   }

   if (count >= 0 && count <= 8){
   Serial.println(count);
   ledON();
   }
   else if ( count > 8){
    count = 0;
   }
   else if (count < 0){
     count = 8;
   }
    
}
