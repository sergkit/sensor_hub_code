#include <Wire.h>
#include "display8x8.h"


#define I2CAddressESPWifi 6

unsigned long loopTime_time,loopTime_sen, currentTime;
unsigned long delayTime_time = 1000, delayTime_sen = 15000;
boolean timeReady=false;
boolean dotter=false;
uint32_t old_ts;

display8_8 d88(12,11,10);

HM cur_t;
SEN cur_sen, cur_sen1;

void setup() {
  Serial.begin(9600);
  d88.displayInit();
  DHT_init();
  set_mhz(4);
  initVOC(false);
  
  Wire.begin(I2CAddressESPWifi);
  Wire.onReceive(espWifiReceiveEvent);
  Wire.onRequest(espWifiRequestEvent);
  
  currentTime = millis();
  loopTime_time = currentTime;
  loopTime_sen = currentTime;
  pinMode(13, OUTPUT);
  digitalWrite(13, dotter);
}

void loop() {
  currentTime = millis();
  if (currentTime >= (loopTime_time + delayTime_time)) {
      if(timeReady && cur_t.valid){
        d88.displayTime(cur_t);
      }
      loopTime_time = millis();
      dotter=!dotter;
      digitalWrite(13, dotter);
  }
  if (currentTime >= (loopTime_sen + delayTime_sen)) {
    loopTime_sen = millis();
      proc();
  }
  delay(1);
}

void proc(){
  
  readTH();
  cur_sen1.co2=(word)getMHZ();
  cur_sen1.t=getT();
  cur_sen1.h=getH();
  cur_sen1.voc=getVOC(cur_sen1.t, cur_sen1.h);
  cur_sen.valid=false;
  cur_sen=cur_sen1;
  cur_sen.valid=true;
  sendSen();
}
void sendSen(){
  Serial.print("T:");
  Serial.print((float)cur_sen.t/100.0);
  Serial.print(" H:");
  Serial.print((float)cur_sen.h/100.0);
  Serial.print(" CO2:");
  Serial.print(cur_sen.co2);
  Serial.print(" voc:");
  Serial.println(cur_sen.voc);
    Serial.print("[");
  uint8_t * b=(uint8_t*)&cur_sen;
  for(int i=0; i<12; i++)  {
    Serial.print(b[i]); Serial.print(", ");
  }
  Serial.println("}");
}
void espWifiReceiveEvent(int count)
{
  uint8_t i=0;
//  Serial.print("Received[");
//  Serial.println(count);
  timeReady=false;
  uint8_t * b=(uint8_t*)&cur_t;
  while (Wire.available())  {
    b[i] = Wire.read();
    i++;
    if (i>5){
      i=0;
    }
  }
  if (cur_t.head1==153 && cur_t.head2==154){
    // Serial.print(cur_t.valid);Serial.print(cur_t.h);Serial.print(":");Serial.println(cur_t.m);
     timeReady=true;
  }else{
    //response="ERROR";
  }
//  Serial.println("]");
  
}
void espWifiRequestEvent(){
  if (cur_sen.valid){
    Wire.write((uint8_t*)&cur_sen,12);
  }else{
    Wire.write(255);
    Wire.write(255);    
  }
}




