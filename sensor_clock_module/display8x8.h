 
#ifndef display8x8
#define display8x8

#include <Arduino.h>

#include "LedControl.h"
#include "Font4x8.h"
#include "display_struc.h"

class display8_8
{
  private:
    void displayHH(byte adr, byte d1, byte d2);
    void displayHH(byte adr, byte i);
    LedControl _lc;
 public:
    display8_8(int pin_in, int pin_clk, int pin_load);
    void displayInit();
    void displayTime(HM t);

};

#endif
