# ifndef WIFI_SETUP_H
# define WIFI_SETUP_H

#include <WiFi.h>
#include <LiquidCrystal.h>

class WiFiData {
private:
    const char* ssid;
    const char* passwd;
public:
    WiFiData(const char* ssid, const char* password)
    {
      this->ssid = ssid;
      this->passwd = password;
    }
    WiFiData(const WiFiData& userdata){
      this->ssid = userdata.ssid;
      this->passwd = userdata.passwd;
    }
    const char* SSID()
    {
        return ssid;
    }
    const char* password()
    {
        return passwd;
    }
};

class WiFiSetup {
private:
  int delayValue;
  WiFiData data;
public:
  WiFiSetup(const WiFiData& userdata, int dv = 300) 
    : delayValue(dv)
    , data(userdata)
    {
  }
  void operator() (LiquidCrystal& lcd) {
    String lcdText;
    char clkSymbolSet[4] = {'|', '/', '-', '\\'};
    WiFi.mode(WIFI_STA);
    WiFi.begin(data.SSID(), data.password());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcdText = "CNT TO ";
    lcdText += data.SSID();
    lcd.print(lcdText);
    lcd.setCursor(0, 1);
    lcd.print("Waiting ");
    for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
        lcd.setCursor(10, 1);
        lcd.write(clkSymbolSet[i%4]);
        delay(delayValue);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcdText);
    lcd.setCursor(0, 1);
    lcd.print("Successful");
    delay(1000);
  }
};

// Add data network here

// Home Network
const WiFiData home("Ryuuba", "xoloitzcuintle");

#endif