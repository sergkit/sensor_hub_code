
#include "display8x8.h"
byte fh48=8; 

display8_8::display8_8(int pin_in, int pin_clk, int pin_load):_lc(pin_in,pin_clk,pin_load,2){
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
}
void display8_8::displayInit(){
  for(int i=0; i<2; i++){
    _lc.shutdown(i,false);
    /* Set the brightness to a medium values */
    _lc.setIntensity(i,4);
    /* and clear the display */
    _lc.clearDisplay(i);
  }
}

void display8_8::displayTime(HM t){
  displayHH(1, t.h);
  displayHH(0, t.m);
}

void display8_8::displayHH(byte adr, byte d1, byte d2){
  byte dots;
  d1-=48; d2-=48;
  Serial.println(d1);
  for (byte col = 0; col < fh48; col++) {
    dots = pgm_read_byte_near(&myfont[d1][col])<<4 | pgm_read_byte_near(&myfont[d2][col]) ;
    _lc.setColumn(adr,7-col,dots);
  }
}

void display8_8::displayHH(byte adr, byte i){
  byte d1, d2;
  byte dots;
  d1=byte(i/10);
  d2=byte(i-d1*10);
  for (byte col = 0; col < fh48; col++) {
    dots = pgm_read_byte_near(&myfont[d1][col])<<4 | pgm_read_byte_near(&myfont[d2][col]) ;
    _lc.setColumn(adr,7-col,dots);
  }
}
