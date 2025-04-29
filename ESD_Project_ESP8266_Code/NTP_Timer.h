 void InitNTPTimer();
 unsigned long GetUnixTime();
 void PrintLocalTime();
 int GetYear();
 int GetMonth();
 int GetDay();
 int GetYearDay();
 int GetWeek();
 int GetHour();
 int GetMinutes();
 int GetSecond();
 bool IsDaylightSavingTime();
 
 #include <time.h>
 
 #define NTP_Server  "pool.ntp.org"
 #define NIST_Server "time.nist.gov"
 
 int8_t TIME_ZONE = 8;             // +8 HK
 const int    daylightOffset_sec = 0;
 
 struct tm timeinfo;
 
 time_t now;
 time_t nowish = 1510592825;
 
 void InitNTPTimer() {
   Serial.print("Setting time using SNTP ");
   configTime(TIME_ZONE * 3600, daylightOffset_sec, NTP_Server, NIST_Server);
   
   now = time(nullptr);
   while (now < nowish)
   {
     delay(500);
     now = time(nullptr);
   }
 
   gmtime_r(&now, &timeinfo);
 }
 
 unsigned long GetUnixTime() {
   if(!getLocalTime(&timeinfo))
   {
     return 0;
   }
   
   time(&now);
   return now;
 }
 
 void PrintLocalTime(){
   String str = "Year : ";
   str += String(GetYear());
   str += "\nMonth : ";
   str += String(GetMonth());
   str += "\nWeek : ";
   str += String(GetWeek());
   str += "\nDay : ";
   str += String(GetDay());
   str += "\nHour : ";
   str += String(GetHour());
   str += "\nMinutes : ";
   str += String(GetMinutes());
   str += "\nSecond : ";
   str += String(GetSecond());
   Serial.println(str);
 }
 
 int GetYear()   { return timeinfo.tm_year + 1900; }
 int GetMonth()  { return timeinfo.tm_mon + 1; }
 int GetWeek()   { return timeinfo.tm_wday; } // （0=Sunday，1=Monday，6=Saturday）
 int GetDay()    { return timeinfo.tm_mday; }
 int GetYearDay(){ return timeinfo.tm_yday + 1; }
 int GetHour()   { return timeinfo.tm_hour; }
 int GetMinutes(){ return timeinfo.tm_min; }
 int GetSecond() { return timeinfo.tm_sec; }
 
 bool IsDaylightSavingTime() { return timeinfo.tm_isdst > 0; } 