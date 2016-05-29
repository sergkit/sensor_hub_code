#include "ntp.h"

NTP::NTP(char* ntpServerName, int timezone){
  _ntpServerName=ntpServerName;
  _TIMEZONE=timezone;
}

void NTP::init(){
  WiFi.hostByName(_ntpServerName, _timeServerIP); 
  _udp.begin(_localPort);
}

unsigned long NTP::GetTime(){
      return _ntp_time; 
    }
/**
 * Посылаем и парсим запрос к NTP серверу
 */
bool NTP::GetNTP(void) {
  sendNTPpacket(_timeServerIP); 
  delay(1000);
  int cb = _udp.parsePacket();
  if (!cb) {
    Serial.println("No packet yet");
    return false;
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
// Читаем пакет в буфер    
    _udp.read(_packetBuffer, _NTP_PACKET_SIZE); 
// 4 байта начиная с 40-го сождержат таймстамп времени - число секунд 
// от 01.01.1900   
    unsigned long highWord = word(_packetBuffer[40], _packetBuffer[41]);
    unsigned long lowWord = word(_packetBuffer[42], _packetBuffer[43]);
// Конвертируем два слова в переменную long
    unsigned long secsSince1900 = highWord << 16 | lowWord;
// Конвертируем в UNIX-таймстамп (число секунд от 01.01.1970
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
// Делаем поправку на местную тайм-зону
    _ntp_time = epoch + _TIMEZONE*3600;    
    Serial.print("Unix time = ");
    Serial.println(_ntp_time);
  }
  return true;
}

/**
 * Посылаем запрос NTP серверу на заданный адрес
 */
unsigned long NTP::sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
// Очистка буфера в 0
  memset(_packetBuffer, 0, _NTP_PACKET_SIZE);
// Формируем строку зыпроса NTP сервера
  _packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  _packetBuffer[1] = 0;     // Stratum, or type of clock
  _packetBuffer[2] = 6;     // Polling Interval
  _packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  _packetBuffer[12]  = 49;
  _packetBuffer[13]  = 0x4E;
  _packetBuffer[14]  = 49;
  _packetBuffer[15]  = 52;
// Посылаем запрос на NTP сервер (123 порт)
  _udp.beginPacket(address, 123); 
  _udp.write(_packetBuffer, _NTP_PACKET_SIZE);
  _udp.endPacket();
}
