// For Testing
#include "ST7735_Header.h"
#include "Buzzer_Header.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "NTP_Timer.h"
#include "StringFunction.h"

///// Other /////

#define EEPROM_SIZE 2

int alarm_hour = 0;
int alarm_mins = 0;

#define BUZZER_PIN 16 // D0

String command = "";

///// WiFi Connection Set up /////
ESP8266WiFiMulti wifiMulti;

char wifi_ssid[]     = "Cabala";
char wifi_password[] = "21424861";
int addressCount = 0;

void setup()
{
  Serial.begin(9600);

  InitST7735();
  PrintText("Set up");

  InitBuzzer(BUZZER_PIN);

  EEPROM.begin(EEPROM_SIZE);

  delay(2000);
  ReadEEPROMSave();
  delay(2000);

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
  command = SerialToStringUntil('\n');  // Read the command from MSP430 RxTx 

  if (command != "") {

    PrintText(command);
    HandleCommand(command);             // Handle the different command
    command = "";                       // clear the command
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

void ReadEEPROMSave(){
  alarm_hour = EEPROM.read(0);
  alarm_mins = EEPROM.read(1);
  if (alarm_hour == 0 && alarm_mins == 0) // If have not save, just send default alarm
  {
    Serial.println("3,20,30");
    PrintText("3,20,30", 20, false);
  }
  else
  {
    String command = "3,";
    command += String(alarm_hour);
    command += ",";
    command += String(alarm_mins);
    Serial.println(command);
    PrintText(command, 20, false);
  }
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

// Command
// 1 -> Set time
// 2 -> On Off Auto update
// 3 -> Set Alarm
// 4 -> On/Off Alarm
// 5 -> Start / Stop Buzzer
void HandleCommand(String command){
  String* commands = SplitString(command, ',', 3);  // split the Command to [command][message][message]]
  
  switch(commands[0].toInt()){
    case 3:                               // 3 -> Set Alarm
      alarm_hour = commands[1].toInt();
      alarm_mins = commands[2].toInt();
      EEPROM.write(0, alarm_hour);
      EEPROM.write(1, alarm_mins);
      EEPROM.commit();                    // save to ROM
      PrintText("Save Alarm Successful", 20, false);
      break;
  }
}
