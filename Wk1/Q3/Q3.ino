int count = 0,right = 1,left =0;

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
    right =0;
    left++;
    count++;
   }     
   else if (analogRead(R) <=600){
    left = 0;
    right++;
   }
   if (right >= 1){moveright();}
   if (left>=1){moveleft();}
}
 void moveright(){
   count--;
   if (count >= 0 && count <= 8){
   ledON();
   }
   else if ( count > 8){
    count = 0;
   }
   else if (count < 0){
     count = 8;
   }
   
}
void moveleft(){
   count++;
   if (count >= 0 && count <= 8){
   ledON();
   }
   else if ( count > 8){
    count = 0;
   }
   else if (count < 0){
     count = 8;
   }
   
}
