byte cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
char res[9] = {'x','x','x','x','x','x','x','x','x'}; 
char response[9]; 
String ppmString = " ";
#define AIQ_SENSOR_ANALOG_PIN A1
const byte MHZ_0=0;
const byte MHZ_DIG= 1;
const byte MHZ_PWM= 2;
const byte MHZ_AN1= 3;
const byte MHZ_AN2= 4;

byte mhz_mode=0;

const word preheat_val=999;
unsigned long preheat_time = 300000;
unsigned long preheat_time_start;
boolean preheat_mode;
int (*getMHZ_)();

void  set_mhz(byte mode) {
  mhz_mode=mode;
  preheat_mode=true;
  preheat_time_start = millis();
  switch (mode) {
    case MHZ_DIG:
      Serial.begin(9600);
      getMHZ_=&get_co2_digital;
     break;
    case MHZ_PWM:
          getMHZ_=&get_co2_PWM;
     break;
     case MHZ_AN1:
          getMHZ_=&get_co2_Analog;
     break;
    case MHZ_AN2:
          getMHZ_=&get_co2_Analog1;
     break;
     case MHZ_0:
          getMHZ_=&get_co2_0;
     break;      
  }

} 


int getMHZ(){
  if (preheat_mode){
    if (preheat_time_start+preheat_time<millis()){
      preheat_mode=false;
    }
    return preheat_val;
  }else{
   return getMHZ_();
  }  
}

int get_co2_0(){
  return 0;
}

int get_co2_digital(){
  Serial.write(cmd,9);
  int resb=Serial.readBytes(response, 9);
  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256*responseHigh)+responseLow;
  return ppm;
}

int get_co2_PWM(){
  while(digitalRead(AIQ_SENSOR_ANALOG_PIN) == HIGH) {;}
    //wait for the pin to go HIGH and measure HIGH time
    unsigned long duration = pulseIn(AIQ_SENSOR_ANALOG_PIN, HIGH);
    //from datasheet
    //CO2 ppm = 2000 * (Th - 2ms) / (Th + Tl - 4ms)
    //  given Tl + Th = 1004
    //        Tl = 1004 - Th
    //        = 2000 * (Th - 2ms) / (Th + 1004 - Th -4ms)
    //        = 2000 * (Th - 2ms) / 1000 = 2 * (Th - 2ms)
    long co2ppm = 5 * ((duration/1000) - 2);
    return (int)co2ppm;
  
}
int get_co2_Analog(){
   long ppm=(analogRead(AIQ_SENSOR_ANALOG_PIN)*5.00/1024-0.4)*5000/1.6;
   return (int)ppm; 
}

int get_co2_Analog1(){
   long ppm=(analogRead(AIQ_SENSOR_ANALOG_PIN)*5.00/1024)*5000/2.5;
   return (int)ppm; 
}

