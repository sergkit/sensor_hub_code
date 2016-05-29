#ifndef display_struc
#define display_struc

struct HM{
  byte head1=153;
  byte head2=154;
  boolean valid=false;
  byte h=0;
  byte m=0; 
  byte st=0; 
};

struct SEN{
  byte head1=155;
  byte head2=156;
  boolean valid=false;
  byte room=2;
  uint16_t t=0;
  uint16_t h=0;
  uint16_t co2;
  uint16_t voc;
};
#endif
