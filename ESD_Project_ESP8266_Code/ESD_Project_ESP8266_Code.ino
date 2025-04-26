// For Testing
#include "ST7735_Header.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "NTP_Timer.h"
#include "StringFunction.h"
#include "FirebaseFunction.h"
#include "FirebaseKey.h"
#include "Buzzer_Header.h"

///// Pin  /////
#define BUZZER_PIN 16 // D0

///// Save Value /////
#define EEPROM_SIZE 2

int alarm_hour = 0;
int alarm_mins = 0;

///// WiFi Connection Set up /////
ESP8266WiFiMulti wifiMulti;

char wifi_ssid[]     = "Cabala";
char wifi_password[] = "21424861";
int addressCount = 0;

///// Firebase setting /////

#define AlarmSavePath "/MSP430_Project/Alarm_Setting"

///// Other /////

String command = "";

unsigned long nextSendTime = 0;
unsigned long nextSendTimeDuration = 20000; // 20 second send one time

void setup()
{
  Serial.begin(9600);

  InitST7735();
  PrintText("Start Set up");

  InitBuzzer(BUZZER_PIN);
  EEPROM.begin(EEPROM_SIZE);

  PrintText("Try Read EEPROM Save", 10, false);
  delay(2000);
  ReadEEPROMSave();
  delay(2000);

  AddWiFiAddress(wifi_ssid, wifi_password);
  ConnectWiFi(30);

  if (WiFi.isConnected())
  {
    InitNTPTimer();
    PrintText("Init NTP Timer Done", 50, false);

    PrintText("Connecting Firebase", 60, false);
    LoginFirebase(API_KEY, DATABASE_URL, USER_EMAIL, USER_PASSWORD);

    PrintText("Connect Firebase Successful.", 70, false);
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

  SendoutTimestamp();
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

void AddWiFiAddress(char* ssid, char* password){
  wifiMulti.addAP(ssid, password);
  addressCount++;
}

void ConnectWiFi(float timeout){
  if (addressCount == 0) return;
  
  float currentTime = 0;

  wifiMulti.run();

  String log = "Connecting WiFi";

  PrintText(log, 30, false);

  while(!WiFi.isConnected() || currentTime < timeout)
  {
    delay(500);
    currentTime += 0.5f;
    log += ".";
    PrintText(log, 30, false);
  }

  if (WiFi.isConnected())
    PrintText("Done Connected", 40, false);
  else
    PrintText("Fail Connect WiFi", 40, false);
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

      if (WiFi.isConnected()){
        String firebaseSave = commands[1];
        firebaseSave += ",";
        firebaseSave += commands[2];
        SetStringToFirebase(AlarmSavePath, firebaseSave);
      }

      break;
  }
}

void SendoutTimestamp(){
  if (millis() > nextSendTime){
    GetUnixTime();                  // update crrent time
    String message = "1,";          // 1 -> Set time

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
    nextSendTime = millis() + nextSendTimeDuration;
  }
}

void SetStringToFirebase(String path, String s){
  if (Firebase.setString(firebaseData, path ,s))
    PrintText("Save Firebase Successful", 30, false);
  else
  {
    PrintText("Save Firebase Fail", 30, false);
    PrintText(firebaseData.errorReason().c_str(), 40, false);
  }
}

/*
String GetString(String path)
{
  Serial.printf("Get string : %s\n", Firebase.getString(firebaseData, path) ? String(firebaseData.stringData()).c_str() : firebaseData.errorReason().c_str());
  
  if(firebaseData.dataType() == "string")
    return firebaseData.stringData();
  else
  {
    printf("[%s] is not string type\n", path.c_str());
    return "";
  }
}
*/