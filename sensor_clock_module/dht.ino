#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            7         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)

float cur_temp, cur_ham;

DHT_Unified dht(DHTPIN, DHTTYPE);

void DHT_init() {
  dht.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
}

void readTH(){
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    //cur_t = -1;
  }
  else {
    cur_temp =event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    //cur_h = -1;
  }
  else {
    cur_ham =event.relative_humidity;
  }  
}

word getT() {
  return (word)(cur_temp*100.0);
}
word getH() {
  return (word)(cur_ham*100.0);
}
