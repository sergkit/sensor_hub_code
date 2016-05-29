#ifndef display_struc
#define display_struc

struct HM{
  byte head1=153;
  byte head2=154;
  boolean valid=false;
  byte h=0;
  byte m=0;  
};

struct SEN{
  byte head1=155;
  byte head2=156;
  boolean valid=false;
  byte room=2;
  word t=0;
  word h=0;
  word co2;
  word voc;
};
#endif
