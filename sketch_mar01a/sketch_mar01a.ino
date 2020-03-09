#define Data 4
#define Latch 3
#define clock 2
int number[3] = {0b00111111,0b00000110,0b01011011};
unsigned char PORTD_data;

void setup(){
  DDRD = 0b11111111;
  Serial.begin(9600);
}

void loop(){
  setled(number[1]);
    delay(1000);
}
void setled(int data]){
    PORTD = 0b00000000;
   Serial.println(data);
    for (int i = 0; i<8 ; i++){
      PORTD_data = 0b00000100 && (data >> 7-i);
        PORTD = 0b00000001 || PORTD_data ;
        PORTD = 0b00000000;
     
  }
    PORTD = 0b00000010; //latch
}
