#include "SerialRead_Header.h"

// For Testing
#include "ST7735_Header.h"
#include "Buzzer_Header.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "NTP_Timer.h"

///// Other /////

#define BUZZER_PIN 16 // D0

String command = "";

///// WiFi Connection Set up /////
ESP8266WiFiMulti wifiMulti;

char wifi_ssid[]     = "Cabala";
char wifi_password[] = "21424861";
int addressCount = 0;

// Command
// 1 -> Set time
// 2 -> On Off Auto update
// 3 -> Set Alarm
// 4 -> On/Off Alarm
// 5 -> Start / Stop Buzzer

void setup()
{
  Serial.begin(9600);

  InitST7735();
  PrintText("Set up");

  InitBuzzer(BUZZER_PIN);

  delay(2000);
  Serial.println("3,20,30");

  AddWiFiAddress(wifi_ssid, wifi_password);
  PrintText("Connecting WiFi");
  ConnectWiFi(30);

  if (WiFi.isConnected())
  {
    InitNTPTimer();
    PrintText("Init NTP Timer Done");
  } else{
    // For Testing
    PrintText("Init Done without Network");
  }
}

void loop()
{
  command = SerialToStringUntil('\n');

  if (command != "") {
    PrintText(command);

    if (command == "alarm_on")
      StartBuzzerLoop(4, 500, 500);
        
    command = "";
  }

  delay(10000);

  String message = "1,";
  message += String(GetYear());
  message += ",";
  message += String(GetMonth());
  message += ",";
  message += String(GetDay());
  message += ",";
  message += String(GetWeek());
  message += ",";
  message += String(GetHour() + TIME_ZONE);
  message += ",";
  message += String(GetMinutes());
  message += ",";
  message += String(GetSecond());

  Serial.println(message);
  PrintText(message);
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
