/* 
 * Compatible ESP32 & ESP8266
 * Get the current time from internet (NTP)
 * 
 * Resource : https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "NTP_Timer.h"

///// WiFi Connection Set up /////
ESP8266WiFiMulti wifiMulti;

char wifi_ssid[]     = "Cabala";
char wifi_password[] = "21424861";
int addressCount = 0;

void setup()
{
  Serial.begin(115200);
  
  AddWiFiAddress(wifi_ssid, wifi_password);
  ConnectWiFi(30);

  if (WiFi.isConnected())
    InitNTPTimer();
}

void loop()
{
  PrintLocalTime();
  Serial.println(GetUnixTime());
  delay(10000);
}


void AddWiFiAddress(char* ssid, char* password)
{
  wifiMulti.addAP(ssid, password);
  addressCount++;
}

void ConnectWiFi(float timeout)
{
  if (addressCount == 0)
  {
    Serial.println("Haven't assigned any WiFi Address.");
    return;
  }

  float currentTime = 0;

  wifiMulti.run();

  Serial.print("Connecting Wifi");

  while(!WiFi.isConnected() || currentTime < timeout)
  {
    delay(500);
    currentTime += 0.5f;
    Serial.print(".");
  }

  if (WiFi.isConnected())
  {
    /* Check the device IP Address */
    Serial.println();
    Serial.println("Connected, IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("Timeout, Fail Connection");
  }
}
