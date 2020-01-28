void setup()
{
  Serial.begin(9600);
  DDRD = 0xFF;
  DDRB = 0b00000011;
}

void loop(){
 
  for ( int i =1;i <=8;i++){
    
    if (i<7){
   PORTB = 0b00000000;
   PORTD = 0b00000010 << i;
   
    }
    else if(i >= 7){
      PORTD = 0b00000000;
      PORTB = 0b00000001 << (i - 7);
     }
    delay(1000);
  }
}
