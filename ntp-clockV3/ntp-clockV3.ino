#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "ntp.h"
#include "display_struc.h"
#include "web_server.h"

#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <Wire.h>
#define I2CAddressESPWifi 6

#include <RtcDS3231.h>
RtcDS3231 Rtc;

#include <Ticker.h>

Ticker ticker_send_time, ticker_send_sen, ticker_get_time, ticker_get_sen;
boolean b_send_time = false, b_send_sen = false, b_get_time = false, b_get_sen = false;

char cur[7] = "990000";
long  t_correct        = 0;
unsigned long cur_ms   = 0;
unsigned long ms1      = 0;
unsigned long ms2      = 10000000UL;
bool          points   = true;
unsigned int err_count = 0;
bool sensorReady = false;
String inputString = "";
unsigned long ntp_time;
char ntp_adr[] = "time.nist.gov";
byte st;

HM cur_t;
SEN cur_sen, cur_sen1;
NTP c_ntp(ntp_adr, 3);

void set_send_time() {
  b_send_time = true;
}
void set_send_sen() {
  b_send_sen = true;
}
void set_get_time() {
  b_get_time = true;
}
void set_get_sen() {
  b_get_sen = true;
}
void setup()
{
  Serial.begin(115200);
  if (checkEeprom() && ConnectWiFi(ssid, pass)) {
    normalInit();
  } else {
    APInit();
  }
}

void normalInit() {
  delay(1000);
  c_ntp.init();
  Wire.begin();
  RTCSetTime();
  ticker_send_time.attach(0.5, set_send_time);
  ticker_send_sen.attach(60, set_send_sen);
  ticker_get_time.attach(3600, set_get_time);
  ticker_get_sen.attach(14, set_get_sen);
}
void loop() {
  if (serv_mode) {
    server.handleClient();
    delay(1);
  } else {
    if ( b_send_time ) {
      b_send_time = false;
      cur_t.valid = false;
      if (!Rtc.IsDateTimeValid()) {
        Serial.print(".");
      } else {
        RtcDateTime now = Rtc.GetDateTime();
        cur_t.valid = true;
        cur_t.h = byte(now.Hour());
        cur_t.m = byte(now.Minute());
        cur_t.st=st;
      }

      SendTime();
    }
    if ( b_send_sen ) {
      b_send_sen = false;
      sendSen();
    }
    if ( b_get_time ) {
      b_get_time = false;
      err_count++;
      RTCSetTime();
      ReciveSen();
    }
    if ( b_get_sen ) {
      b_get_sen = false;
      ReciveSen();
    }
    // Если нет соединения с интернетом, перезагружаемся
    if ( err_count > 10 ) {
      Serial.println("NTP connect false");
      Serial.println("Reset ESP8266 ...");
      ESP.reset();
    }
    delay(100);
  }
}

/**
 * Отправка запроса на сервер
 */
void sendSen() {
  Serial.print("T:");
  Serial.print((float)cur_sen.t / 100.0);
  Serial.print(" H:");
  Serial.print((float)cur_sen.h / 100.0);
  Serial.print(" CO2:");
  Serial.print(cur_sen.co2);
  Serial.print(" voc:");
  Serial.println(cur_sen.voc);
  send_request(print_str());
}
/*
 * запись в ардуино
 */
void SendTime() {
  Wire.beginTransmission(I2CAddressESPWifi);
  Wire.write((uint8_t*)&cur_t, 5);
  Wire.endTransmission();
}
/*
 * чтение ИЗ ардуино
 */
void ReciveSen() {
  delay(1);//Wait for Slave to calculate response.
  Wire.requestFrom(I2CAddressESPWifi, 12);
  ReceiveEvent();
}
void ReceiveEvent() {
  uint8_t i = 0;

  uint8_t * b = (uint8_t*)&cur_sen1;
  while (Wire.available())  {
    b[i] = Wire.read();
    i++;
    if (i > 12) {
      i = 0;
    }
  }
  if (cur_sen1.head1 == 155 && cur_sen1.head2 == 156) {
    sensorReady = false;
    cur_sen = cur_sen1;
    sensorReady = true;
  }
}

/**
 * Установка RTC по NTP
 */
void RTCSetTime() {
  if (c_ntp.GetNTP()) {
    ntp_time = c_ntp.GetTime();
    //результат в переменной ntp_time
    Rtc.SetDateTime(ntp_time);
    err_count = 0;
    DisplayTime();
  }
}
/**
 * Соединение с WiFi
 */
bool ConnectWiFi(const char *ssid, const char *pass) {
  Serial.println("wifi start");
  // Три попытки соединения по WiFi
  for ( int i = 0; i < 3; i++) {
    WiFi.begin(ssid, pass);
    delay(1000);
    Serial.println(i);
    // Максиммум 12 раз проверка соединения
    for ( int j = 0; j < 12; j++ ) {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("wifi connect");
        return true;
      }
      delay(1000);
    }
  }
  Serial.println("wifi connect false");
  return false;
}

/**
 * Печать текущего времени
 */
void DisplayTime(void) {
  uint16_t m = ( ntp_time / 60 ) % 60;
  uint16_t h = ( ntp_time / 3600 ) % 24;
  Serial.print("Time: ");
  Serial.print(h);
  Serial.print(":");
  Serial.println(m);

}

byte send_request( String data) {
  char ch1 = '\x0', ch2 = '\x0', ch3 = '\x0', ch = '\x0';
  WiFiClient client; // Use WiFiClient class to create TCP connections
  Serial.println(data);
  if (client.connect(url, 80)) {
    Serial.println("connect-send");
    client.println("POST /add_record HTTP/1.1");
    client.println("Host: " + String(url)); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
    delay(2000);
    while (client.available()) {
      ch1 = ch2;
      ch2 = ch;
      ch = client.read();
      if (ch1 == '#' && ch2 == '#') {
        ch3 = ch;
      }
    }


  } else {
    Serial.println("connection failed");
    err_count++;
    return ;
  }

  if (client.connected()) {
    client.stop();  // DISCONNECT FROM THE SERVER
  }

  return ch3 - 48;
}


String print_str() {
  String s;
  s += add_par("add_data[room]=", String(room));
  s += add_par("&add_data[co2]=", String(cur_sen.co2));
  s += add_par("&add_data[t]=", String((float)cur_sen.t / 100.0));
  s += add_par("&add_data[h]=", String((float)cur_sen.h / 100.0));
  s += add_par("&add_data[voc]=", String(cur_sen.voc));
  s += add_par("&add_data[vocr]=", String(cur_sen.voc));
  s += add_par("&add_data[vocold]=", String(cur_sen.voc));
  s += add_par("&add_data[CheckFields]=", String((float)cur_sen.t / 100.0 + cur_sen.co2 + (float)cur_sen.h / 100.0 + cur_sen.voc));
  return s;
}

String add_par(String par, String val) {
  return par + val;
}


