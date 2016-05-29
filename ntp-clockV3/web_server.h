#include <Ticker.h>

const char WiFiAPPSK[] = "ntpclock";
ESP8266WebServer server(80);
const char* serverIndex = "<form action='setup' method='post'>ssid: <input name='ssid'/><br><br>pwd:  <input name='pwd'/><br><br>url:  <input name='url'/><br><br>room: <input name='room'/><br><br><input type='submit' value='Setup'></form>";
byte len_ssid, len_pass,len_url, room;

unsigned char* buf_ssid = new unsigned char[32];
unsigned char* buf_pass = new unsigned char[64];
unsigned char* buf_url = new unsigned char[96];
const char *ssid  = (const char*)buf_ssid;
const char *pass  = (const char*)buf_pass;
const char *url  = (const char*)buf_url;

bool serv_mode = false;
Ticker ticker_error;

bool checkEeprom() {
  Serial.println("CheckEEPROM: ");
  EEPROM.begin(196);
  len_ssid = EEPROM.read(192);
  len_pass = EEPROM.read(193);
  len_url = EEPROM.read(194);
  room = EEPROM.read(195);
  if (len_pass > 64) len_pass = 0;
  if ((len_ssid < 33) && (len_ssid != 0)) {
    for (byte i = 0; i < len_ssid; i++) buf_ssid[i] = char(EEPROM.read(i));
    buf_ssid[len_ssid] = '\x0';
    for (byte i = 0; i < len_pass; i++) buf_pass[i] = char(EEPROM.read(i + 32));
    buf_pass[len_pass] = '\x0';
    for (byte i = 0; i < len_url; i++) buf_url[i] = char(EEPROM.read(i + 32 + 64));
    buf_url[len_url] = '\x0';
    Serial.print("SSID: ");
    Serial.print(ssid);
    Serial.print("   ");
    Serial.print("Password: ");
    Serial.println(pass);
    Serial.print("url: ");
    Serial.print(url);
    Serial.print("   ");
    Serial.print("room: ");
    Serial.println(room);
    return true;
  } else {
    return false;
  }
}
void setupAPWiFi()
{
  Serial.print("AP begin: ");
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;
  Serial.println(AP_NameString);
  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i = 0; i < AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
  serv_mode = true;
}
void esp_reset() {
  ESP.restart();

}

void APInit() {
  setupAPWiFi();
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/setup", HTTP_POST, []() {
    String ssid_ap;
    String pass_ap, url_ap, room_ap;
    unsigned char* buf = new unsigned char[64];

    String str = "";

    EEPROM.begin(196);

    ssid_ap = server.arg("ssid");
    pass_ap = server.arg("pwd");

    url_ap = server.arg("url");
    room_ap = server.arg("room");

    if (ssid_ap != "") {
      EEPROM.write(192, ssid_ap.length());
      EEPROM.write(193, pass_ap.length());
      EEPROM.write(194, url_ap.length());
      EEPROM.write(195, (byte)room_ap[0]-48); //берем первый символ и надеемся, что он цифра
      ssid_ap.getBytes(buf, ssid_ap.length() + 1);
      for (byte i = 0; i < ssid_ap.length(); i++)
        EEPROM.write(i, buf[i]);
      pass_ap.getBytes(buf, pass_ap.length() + 1);
      for (byte i = 0; i < pass_ap.length(); i++)
        EEPROM.write(i + 32, buf[i]);
      url_ap.getBytes(buf, url_ap.length() + 1);
      for (byte i = 0; i < url_ap.length(); i++)
        EEPROM.write(i + 96, buf[i]);
      EEPROM.commit();
      EEPROM.end();
    }


    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", ssid_ap);
    ESP.restart();
  });
  server.begin();
  ticker_error.attach(300, esp_reset);

}


