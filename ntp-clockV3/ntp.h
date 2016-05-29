#ifndef ntp_h
#define ntp_h

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


class NTP
{
  private:
    unsigned long sendNTPpacket(IPAddress& address);
    
    unsigned int  _localPort = 2390;      // local port to listen for UDP packets
    unsigned long _ntp_time = 0;
    char* _ntpServerName = "time.nist.gov";
    static const int _NTP_PACKET_SIZE = 48;
    int _TIMEZONE=3;
    byte _packetBuffer[_NTP_PACKET_SIZE];
    IPAddress _timeServerIP;
    WiFiUDP _udp;
 public:
    NTP(char* ntpServerName, int timezone);
    void init();
    bool GetNTP(void);
    unsigned long GetTime();
};

#endif

