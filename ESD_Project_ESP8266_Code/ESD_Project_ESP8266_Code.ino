#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "NTP_Timer.h"
#include "StringFunction.h"
#include "FirebaseFunction.h"
#include "FirebaseKey.h"
#include "Buzzer.h"
#include "ST7735.h"

///// Pin  /////
#define BUZZER_PIN 16 // D0

///// Save Value /////
#define EEPROM_SIZE 2

int alarm_hour = 0;
int alarm_mins = 0;

///// WiFi Connection Set up /////
ESP8266WiFiMulti wifiMulti;

char wifi_ssid_2[]     = "A33";
char wifi_password_2[] = "12345678";
char wifi_ssid[]     = "Cabala";
char wifi_password[] = "21424861";
int addressCount = 0;

///// Firebase setting /////

#define AlarmSavePath "/MSP430_Project/Alarm_Setting"   //
#define TimeZonePath  "/MSP430_Project/TimeZone"        // TIME_ZONE

///// Other /////

String command = "";

bool onBuzzer = false;

unsigned long nextSendTime = 0;
unsigned long nextSendTimeDuration = 20000; // 20 second send one time

unsigned long nextBuzzerOnTime = 0;
unsigned long nextBuzzerOnTimeDuration = 4000; // 4 second start ont time

void setup()
{
  Serial.begin(9600);
  onBuzzer = false;

  InitST7735();
  PrintText("Start Set up");

  InitBuzzer(BUZZER_PIN);
  EEPROM.begin(EEPROM_SIZE);

  delay(4000);
  PrintText("Try Read EEPROM Save", 10, false);
  ReadEEPROMSave();
  delay(1000);

  AddWiFiAddress(wifi_ssid_2, wifi_password_2);
  AddWiFiAddress(wifi_ssid, wifi_password);
  ConnectWiFi(30);

  if (WiFi.isConnected())
  {
    PrintText("Connecting Firebase", 50, false);
    LoginFirebase(API_KEY, DATABASE_URL, USER_EMAIL, USER_PASSWORD);
    PrintText("Connected Firebase", 60, false);

    UpdateSettingFromFirebase();

    InitNTPTimer();
    PrintText("Init NTP Timer Done", 70, false);
  }
}

void loop()
{
  UpdateBuzzer();

  command = SerialToStringUntil('\n');  // Read the command from MSP430 RxTx 

  if (command != "")
  {
    PrintText(command);
    HandleCommand(command);             // Handle the different command
    command = "";                       // clear the command
  }

  if (onBuzzer)
    HandleBuzzer();

  if (millis() > nextSendTime)
  {
    UpdateSettingFromFirebase();
    SendoutTimestamp();
    nextSendTime = millis() + nextSendTimeDuration;
  }
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
    delay(5000);
    currentTime += 5.0f;
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
      PrintText("Save Alarm Successful", 10, false);

      if (WiFi.isConnected()){
        String firebaseSave = commands[1];
        firebaseSave += ",";
        firebaseSave += commands[2];
        SetStringToFirebase(AlarmSavePath, firebaseSave);
      }

      break;
    case 4:
      if (commands[1] == "on")
      {
        onBuzzer = true;
        PrintText("Buzzer On", 10, false);
      }
      else
      {
        onBuzzer = false;
        PrintText("Buzzer Off", 10, false);
        StopBuzzerLoop();
      }
      break;
  }
}

void HandleBuzzer() {
  if (millis() > nextBuzzerOnTime)
  {
    StartBuzzerLoop(8, 50, 50, 3);
    nextBuzzerOnTime = millis() + nextBuzzerOnTimeDuration;
  }
}

void SendoutTimestamp(){
  GetUnixTime();                  // update crrent time
  String message = "1,";          // 1 -> Set time

  message += String(GetYear());
  message += ",";
  message += String(GetMonth());
  message += ",";
  message += String(GetDay());
  message += ",";

  // in MSP430 is 1 to 7 Mon to Sun, in timestamp 0 is Sun, so need force set to 7
  if (GetWeek() == 0)
    message += "7";
  else
    message += String(GetWeek());

  message += ",";
  message += String(GetHour());
  message += ",";
  message += String(GetMinutes());
  message += ",";
  message += String(GetSecond());

  Serial.println(message);
  ClearScreenFromPosition(100);
  PrintText(message, 100, false);
}

void UpdateSettingFromFirebase(){
  String alarmSetting = GetStringFromFirebase(AlarmSavePath);
  String* alarmNumber = SplitString(alarmSetting, ',', 2);  // split the Command to [message][message]

  int hour = alarmNumber[0].toInt();
  int mins = alarmNumber[1].toInt();

  // update new alarm setting
  if (hour != alarm_hour || mins != alarm_mins){
    alarm_hour = hour;
    alarm_mins = mins;

    // send out update MSP430
    String command = "3,";
    command += alarmSetting;
    Serial.println(command);

    // update local save
    EEPROM.write(0, alarm_hour);
    EEPROM.write(1, alarm_mins);
    EEPROM.commit();

    String log = "Update Alarm : ";
    log += alarmSetting;
    ClearScreenFromPosition(80);
    PrintText(log, 80, false);
  }

  delay(1000);

  // update time zone
  String timeZone = GetStringFromFirebase(TimeZonePath);
  if (timeZone != "")
  {
    int zone = timeZone.toInt();
    if (zone != TIME_ZONE)
    {
      TIME_ZONE = zone;
      InitNTPTimer();
      timeZone = "Update Time zone to " + timeZone;
      ClearScreenFromPosition(90);
      PrintText(timeZone, 90, false);
    }
  }
}

void SetStringToFirebase(String path, String s){
  if (Firebase.setString(firebaseData, path ,s))
    PrintText("Save Firebase Successful", 20, false);
  else
  {
    PrintText("Save Firebase Fail", 20, false);
    PrintText(firebaseData.errorReason().c_str(), 30, false);
  }
}

String GetStringFromFirebase(String path)
{
  Firebase.getString(firebaseData, path);
    
  if(firebaseData.dataType() == "string"){
    return firebaseData.stringData();
  }
  else
  {
    return "";
  }
}